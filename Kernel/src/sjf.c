#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <pcb.h>
#include "listas.h"
#include "semaforos.h"
#include "planificador_mediano_plazo.h"
#include "planificador_largo_plazo.h"
#include "var_glob.h"
#include "sjf.h"

bool menor_estimacion(void* pcb_aux,void* pcb_aux2){
	pcb_t* pcb1 =pcb_aux;
	pcb_t* pcb2 = pcb_aux2;
	return pcb1->estimado_rafaga<pcb2->estimado_rafaga;
}

void remover_de_lista_sjf(t_list* lista,pcb_t* pcb){
	for(int i=0;i<list_size(lista);i++){
		pcb_t* pcb_aux= list_get(lista,i);
		if(pcb_aux->pid == pcb->pid){
			list_remove(lista,i);
			return;
		}
	}
}

void mostrar_lista_ready_sjf(t_list* lista){
	int i;
	int lista_size= list_size(lista);
	log_trace(PCP,"LISTA READY");
	for(i=0;i<lista_size;i++){
		pcb_t* pcb= malloc(sizeof(pcb_t));
		pcb= list_get(lista,i);
		log_trace(PCP,"PCB || PID = %d || prioridad= %d || ESTIMACION: %d  \n", pcb->pid,i, pcb->estimado_rafaga);
	}
}

pcb_t* recibir_paquete_pcb_sjf(){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));
	recv(socket_cpu_dispatch,&paquete->codigo_operacion,sizeof(uint8_t),0);
	recv(socket_cpu_dispatch,&buffer->size,sizeof(uint32_t),0);
	buffer->stream = malloc(buffer->size);
	recv(socket_cpu_dispatch,buffer->stream,buffer->size,0);
	pcb_t* pcb = pcb_deserializar(buffer);
	log_trace(PCP,"recibi un proceso ID: %d ",pcb->pid);
	return pcb;
}

void* enviar_a_ejecutar_sjf(){
	while(1){
		sem_wait(&s_cpu);
		sem_wait(&s_proceso_ejecutando);

		log_trace(PCP,"KERNEL-CPU-PCB Se procede a remover de la cola listos a un proceso para luego enviarlo a ejecutar");

		pthread_mutex_lock(&mx_ready_l);
		pcb_t* pcb= list_remove(ready_l,0);
		pthread_mutex_unlock(&mx_ready_l);

		uint32_t espacio;

		log_trace(PCP,"KERNEL-CPU-PCB Se va a mandar a ejecutar un proceso %d", pcb->pid);

		log_trace(PCP,"KERNEL-CPU-PCB Inicializo el reloj para tomar el tiempo de ejecucion y poder luego estimarlo");
		tiempo_de_ejecucion_inicial = clock();

		void* a_enviar= pcb_serializar(pcb,&espacio,0);

		send(socket_cpu_dispatch,a_enviar,espacio,0);

		pthread_mutex_lock(&mx_proceso_ejecutando);
		proceso_ejecutando=1;
		pthread_mutex_unlock(&mx_proceso_ejecutando);


	}
	return NULL;
}

void* interrupciones()
{
	uint8_t intr = SOLICITAR_INTERRUPCION;
	log_info(PCP,"KERNEL-CPU-PCB Se levanto el Thread que maneja las Interrupciones");
	while(1)
	{
		sem_wait(&s_interrupcion);

		send(socket_cpu_interrupt,&intr,sizeof(uint8_t),0);
		log_info(PCP,"KERNEL-CPU-PCB Se envia Interrupcion a la CPU");
	}
}

void pcb_mostrar_(pcb_t* pcb, t_log* logger)
{
	char* estados[5]={"INICIADO", "BLOQUEADO", "FINALIZADO","SUSPENDIDO","INTERRUMPIDO"};
	printf("\n\nINFORMACION PCB:\n");
	log_info(logger, "INFORMACION PCB:");
	printf("PID: %d\n", pcb->pid);
	log_info(logger, "PID: %d\n", pcb->pid);
	printf("TAMANO: %d\n", pcb->tamano);
	log_info(logger, "TAMANO: %d\n", pcb->tamano);
	printf("PC: %d\n", pcb->program_counter);
	log_info(logger, "PC: %d\n", pcb->program_counter);
	printf("ESTIMADO_RAFAGA: %d\n", pcb->estimado_rafaga);
	log_info(logger, "ESTIMADO_RAFAGA: %d\n", pcb->estimado_rafaga);
	printf("ESTADO: %s\n", estados[pcb->estado]);
	log_info(logger, "ESTADO: %s\n", estados[pcb->estado]);
	printf("TIEMPO BLOQUEO: %d\n", pcb->tiempo_block);
	log_info(logger, "TIEMPO BLOQUEO: %d\n", pcb->tiempo_block);
}


void actualizar_estimacion(pcb_t* pcb){
	log_trace(PCP, "KERNEL-CPU-PCB PCB || PID: %d || ESTIMACION ANTIGUA: %d",pcb->pid,pcb->estimado_rafaga);
	log_trace(PCP, "KERNEL-CPU-PCB ALPHA: %f",alpha);
	pcb->estimado_rafaga = pcb->estimado_rafaga*(1-alpha)+alpha*tiempo_de_ejecucion;
	log_trace(PCP, "KERNEL-CPU-PCB PCB || PID: %d || ESTIMACION ACTUAL: %d",pcb->pid,pcb->estimado_rafaga);
	pcb_mostrar(pcb,PCP);
//	pcb_mostrar_(pcb,PCP);
}

void* bloquear_proceso_sjf(void* pcb_){
	pcb_t* pcb= pcb_;

	//if(pcb->tiempo_block>TIEMPO_BLOCK_MAX){ // CONDICION DE SUSPENSION
	if(pcb->tiempo_block > configuracion->TIEMPO_BLOCK_MAX){
		log_trace(PCP,"KERNEL Se va a suspender un proceso por %d, ID: %d", pcb->tiempo_block,pcb->pid);

		pthread_mutex_lock(&mx_block_l);
		remover_de_lista_sjf(block_l,pcb);
		pthread_mutex_unlock(&mx_block_l);

		log_trace(PCP, "LA PUTA MADREE");
		pthread_mutex_lock(&mx_susp_block_buffer_l);
		list_add(susp_block_buffer_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_buffer_l);

		log_trace(PCP, "LA PUTA MADREE2");
		log_trace(PCP,"KERNEL SEN 1");

		sem_post(&s_proceso_susp);

		log_trace(PCP,"KERNEL SEN 2");

		sem_wait(&s_susp);
		log_trace(PCP,"SALI");


/*
		//	Envio mensaje de suspencion a Memoria
		uint8_t mensaje = SUSPENDER_PROCESO;
		send(socket_memoria, &mensaje, sizeof(uint8_t), 0);

		//	Envio el PID del proceso a suspender
		uint32_t pid = pcb->pid;
		send(socket_memoria,&pid, sizeof(uint32_t),0);
*/
		usleep(pcb->tiempo_block);


		log_info(PCP,"KERNEL Se DESBLOQUEO el proceso suspendido PID:  %d", pcb->pid);
		pthread_mutex_lock(&mx_susp_block_l);
		remover_de_lista_sjf(susp_block_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_l);


		pthread_mutex_lock(&mx_susp_ready_l);
		list_add(susp_ready_l,pcb);
		pthread_mutex_unlock(&mx_susp_ready_l);



		sem_post(&s_proceso_susp_ready);
	}
	else{
		log_info(PCP,"KERNEL Se va a bloquear el proceso PID: %d", pcb->pid);

		usleep(pcb->tiempo_block);




		pthread_mutex_lock(&mx_block_l);
		remover_de_lista_sjf(block_l,pcb);
		pthread_mutex_unlock(&mx_block_l);


		pthread_mutex_lock(&mx_unblock_l);
		list_add(unblock_l,pcb);
		pthread_mutex_unlock(&mx_unblock_l);


		sem_post(&s_proceso_ready);
	}
	return NULL;
}

void* devoluciones(){
	while(1){
		pcb_t* pcb=recibir_paquete_pcb_sjf();
		tiempo_de_ejecucion_final=clock();
		if(tiempo_de_ejecucion_final<0){
//			printf("error en el clock final");
			log_trace(PCP, "KERNEL Error en el tiempo del clock");
		}
		pthread_mutex_lock(&mx_proceso_ejecutando);
		proceso_ejecutando=0;
		pthread_mutex_unlock(&mx_proceso_ejecutando);
		tiempo_de_ejecucion= (tiempo_de_ejecucion_final - tiempo_de_ejecucion_inicial)/(CLOCKS_PER_SEC); // divido por mil para dejarlo en milisegundos
		actualizar_estimacion(pcb);
		if(pcb->estado==INTERRUMPIDO) {
			log_trace(PCP, "KERNEL-CPU-PCB  PCB || PID: %d || MOTIVO: INTERRUMPIDO || SE VA A AGREGAR A READY",pcb->pid);

			pthread_mutex_lock(&mx_interrumpidos_l);
			list_add_sorted(interrumpidos_l, pcb, menor_estimacion);
			pthread_mutex_unlock(&mx_interrumpidos_l);

			sem_post(&s_interrupcion_atendida);
			sem_post(&s_cpu);

		}
		else if(pcb->estado == BLOQUEADO){
			pthread_mutex_lock(&mx_block_l);
			list_add(block_l,pcb);
			pthread_mutex_unlock(&mx_block_l);

				// CREAR EL HILO QUE SE VA A BLOQUEAR UN EL PCB ADENTRO
			log_trace(PCP, "KERNEL-CPU-PCB PCB || PID: %d || MOTIVO: BLOQUEADO || SE VA A BLOQUEAR",pcb->pid);

			pthread_t hilo_bloqueante;
			pthread_create(&hilo_bloqueante,NULL,bloquear_proceso_sjf,pcb);
			pthread_detach(hilo_bloqueante);
		}
		else if(pcb->estado == FINALIZADO){
			log_trace(PCP, "KERNEL-CPU-PCB PCB || PID: %d || MOTIVO: FINALIZADO || SE VA A FINALIZAR",pcb->pid);
			pthread_mutex_lock(&mx_finalizado_l);
			list_add(finalizado_l,pcb);
			pthread_mutex_unlock(&mx_finalizado_l);
			sem_post(&s_proceso_finalizado);
		}
		else{
			log_trace(PCP,"KERNEL-CPU-PCB HAY ERROR");
		}
		sem_post(&s_proceso_ejecutando);
	}
	return NULL;
}

void interrumpir(){
	uint8_t intr = 25;
	send(socket_cpu_interrupt,&intr,sizeof(uint8_t),0);
}

void* agregar_a_ready_sjf(){
	while(1){
		sem_wait(&s_proceso_ready);
		if(proceso_ejecutando){
			log_trace(PCP, "------------------------------------VOY A INTERRUMPIR-------------------------------------------------------------");
			sem_post(&s_interrupcion);
			sem_wait(&s_interrupcion_atendida);
			pcb_t* pcb = list_remove(interrumpidos_l,0);
			list_add_sorted(ready_l,pcb,menor_estimacion);
		}
		pcb_t* pcb;
		if(!list_is_empty(susp_readyM_l)){
			pthread_mutex_lock(&mx_susp_readyM_l);
			pcb= list_remove(susp_readyM_l,0);
			pthread_mutex_unlock(&mx_susp_readyM_l);
		}
		else if(!list_is_empty(unblock_l)){
			pthread_mutex_lock(&mx_unblock_l);
			pcb= list_remove(unblock_l,0);
			pthread_mutex_unlock(&mx_unblock_l);
		}
		else if(!list_is_empty(newM_l)){
			pthread_mutex_lock(&mx_newM_l);
			pcb= list_remove(newM_l,0);
			pthread_mutex_unlock(&mx_newM_l);
		}
		log_trace(PCP,"KERNEL-CPU-PCB INGRESO UN PROCESO A READY, PID: %d", pcb->pid);

		pthread_mutex_lock(&mx_ready_l);
		list_add_sorted(ready_l, pcb, menor_estimacion);
		pthread_mutex_unlock(&mx_ready_l);

		log_trace(PCP,"KERNEL-CPU-PCB HICE EL POST");

		mostrar_lista_ready_sjf(ready_l);

		sem_post(&s_cpu);


	}
	return NULL;
}

void* hablar_con_cpu(){
	pthread_t hilo1;
	pthread_t hilo2;
	pthread_t hilo3;

	pthread_create(&hilo1,NULL,interrupciones,NULL);
	pthread_create(&hilo3,NULL,enviar_a_ejecutar_sjf,NULL);
	pthread_create(&hilo2,NULL,devoluciones,NULL);
	pthread_join(hilo1,NULL);
	pthread_join(hilo2,NULL);
	pthread_join(hilo3,NULL);
	return NULL;
}



void* sjf(){
	pthread_t hilo1;
	pthread_t hilo2;

	pthread_create(&hilo1,NULL,agregar_a_ready_sjf,NULL);
	pthread_create(&hilo2,NULL,hablar_con_cpu,NULL);

	pthread_join(hilo1,NULL);
	pthread_join(hilo2,NULL);
	return NULL;

}
