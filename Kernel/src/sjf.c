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
	pcb_t* pcb2 =pcb_aux2;
	if(pcb1->estimado_rafaga == pcb2->estimado_rafaga){
		return true;
	}
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
	log_info(logger,"PCP || ---------- LISTA DE READY SRT ----------");
	for(i=0;i<lista_size;i++){
		pcb_t* pcb= malloc(sizeof(pcb_t));
		pcb= list_get(lista,i);
		log_trace(logger,"PCP || PID = %d\t||Prioridad= %d\t||ESTIMACION: %d", pcb->pid,i, pcb->estimado_rafaga);
	}
}

pcb_t* recibir_paquete_pcb_sjf(){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));
	recv(socket_cpu_dispatch,&paquete->codigo_operacion,sizeof(uint8_t),0);
	recv(socket_cpu_dispatch,&buffer->size,sizeof(uint32_t),0);
	buffer->stream = malloc(buffer->size);
	recv(socket_cpu_dispatch,buffer->stream,buffer->size,0);
	pcb_t* pcb = pcb_deserializar(buffer, logger);
	log_trace(logger,"PCP || Recibi un proceso con PID: %d ",pcb->pid);
	return pcb;
}

void* enviar_a_ejecutar_sjf(){
	while(1){
		sem_wait(&s_cpu);
		sem_wait(&s_proceso_ejecutando);

		log_trace(logger,"PCP || PCP-EJECUTAR Me preparo para enviar a ejecutar al CPU un proceso");

		pthread_mutex_lock(&mx_ready_l);
		pcb_t* pcb= list_remove(ready_l,0);
		pthread_mutex_unlock(&mx_ready_l);

		uint32_t espacio;

		log_info(logger,"PCP || KERNEL-CPU-PCB Se va a mandar a ejecutar el proceso con PID %d", pcb->pid);

		tiempo_de_ejecucion_inicial = time(NULL);

		void* a_enviar= pcb_serializar(pcb,&espacio,0);

		pthread_mutex_lock(&mx_proceso_ejecutando);
		send(socket_cpu_dispatch,a_enviar,espacio,0);
		proceso_ejecutando=1;
		pthread_mutex_unlock(&mx_proceso_ejecutando);


	}
	return NULL;
}

void* interrupciones(){
	uint8_t intr = 25;
	while(1){
		sem_wait(&s_interrupcion);

		send(socket_cpu_interrupt,&intr,sizeof(uint8_t),0);
	}
}

void actualizar_estimacion(pcb_t* pcb){
	log_info(logger,"PCP || ---------- ACTUALIZACION DE ESTIMACION PROCESO POR SRT ----------");
	log_trace(logger, "PCP || PID: %d\t||ESTIMACION ANTIGUA: %d",pcb->pid,pcb->estimado_rafaga);
	log_trace(logger, "PCP || ALPHA: %f\t||TIEMPO DE EJECUCION: %f",alpha,tiempo_de_ejecucion);
	log_trace(logger, "PCP || ESTIMACION EN FLOTANTE: %f",(tiempo_de_ejecucion*(1-alpha)*1000)+alpha*((double) pcb->estimado_rafaga));
	pcb->estimado_rafaga = (uint32_t) (tiempo_de_ejecucion*(1-alpha)*1000+alpha*pcb->estimado_rafaga);
	log_trace(logger, "PCP || PID: %d\t||ESTIMACION ACTUAL: %d",pcb->pid,pcb->estimado_rafaga);
//	pcb_mostrar(pcb,logger);
}

void* bloquear_proceso_sjf(void* pcb_){
	pcb_t* pcb= pcb_;

	if(pcb->tiempo_block>TIEMPO_BLOCK_MAX){ // CONDICION DE SUSPENSION
		log_info(logger,"PCP || PCP-SUSPENSION Se va a suspender el proceso con PID: %d por %d",pcb->tiempo_block,pcb->pid);

		pthread_mutex_lock(&mx_block_l);
		remover_de_lista_sjf(block_l,pcb);
		pthread_mutex_unlock(&mx_block_l);

		pthread_mutex_lock(&mx_susp_block_buffer_l);
		list_add(susp_block_buffer_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_buffer_l);


		sem_post(&s_proceso_susp);

		sem_wait(&s_susp);


		usleep(pcb->tiempo_block);


		log_info(logger,"PCP || PCP-SUSPENSION Se quita el proceso con PID %d de suspendido", pcb->pid);
		pthread_mutex_lock(&mx_susp_block_l);
		remover_de_lista_sjf(susp_block_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_l);


		pthread_mutex_lock(&mx_susp_ready_l);
		list_add(susp_ready_l,pcb);
		pthread_mutex_unlock(&mx_susp_ready_l);



		sem_post(&s_proceso_susp_ready);
	}
	else{
		//VER BIEN ESTE MENSAJE DEL LOG PORQUE NO SE SI ES CORRECTO
		log_trace(logger,"PCP || PCP-SUSPENSION se va a bloquear un proceso por, ID: %d", pcb->pid);

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
		log_trace(logger,"PCP || KERNEL-CPU-PCB Recibi PCB desde la CPU");
		pthread_mutex_lock(&mx_proceso_ejecutando);
		proceso_ejecutando=0;
		pthread_mutex_unlock(&mx_proceso_ejecutando);
		tiempo_de_ejecucion_final=time(NULL);
		if(tiempo_de_ejecucion_final<0){
			log_trace(logger,"PCP || Error en el calculo del tiempo de ejecucion ya que me da negativo");
//			printf("error en el clock final");
		}
		tiempo_de_ejecucion= difftime(tiempo_de_ejecucion_final,tiempo_de_ejecucion_inicial); // divido por mil para dejarlo en milisegundos
		actualizar_estimacion(pcb);
		log_trace(logger,"PCP || ------------------------ MI ESTADO ES: %d-------------------------------------", pcb->estado);
		//	Valido el estado del PCB que fue devuelto desde el CPU
		if(pcb->estado==INTERRUMPIDO) {
			log_trace(logger, "PCP || PID: %d\t||MOTIVO: INTERRUMPIDO\t||SE VA A AGREGAR A READY",pcb->pid);
			//	Agrego el PCB devuelto a la lista de interrumpidos
			pthread_mutex_lock(&mx_interrumpidos_l);
			list_add(interrumpidos_l, pcb);
			pthread_mutex_unlock(&mx_interrumpidos_l);
			//	Incremento el semaforo de interrupcion atendida para dejar asentado que se realizo la misma
			sem_post(&s_interrupcion_atendida);
			sem_wait(&s_espero_replanificacion);
		}
		else if(pcb->estado == BLOQUEADO){
			//	Agrego el PCB a la lista de procesos bloqueados
			pthread_mutex_lock(&mx_block_l);
			list_add(block_l,pcb);
			pthread_mutex_unlock(&mx_block_l);

				// CREAR EL HILO QUE SE VA A BLOQUEAR UN EL PCB ADENTRO
			log_trace(logger, "PCP || PID: %d\t||MOTIVO: BLOQUEADO\t||SE VA A BLOQUEAR",pcb->pid);
			//	Incremento el semaforo de IO Pendiente para que actue el planificador de IO
			sem_post(&s_io_pendiente);
		}
		else if(pcb->estado == FINALIZADO){
			log_trace(logger, "PCP || PID: %d\t||MOTIVO: FINALIZADO\t||SE VA A FINALIZAR",pcb->pid);
			//	Agrego el PCB a la lista de procesos finalizados
			pthread_mutex_lock(&mx_finalizado_l);
			list_add(finalizado_l,pcb);
			pthread_mutex_unlock(&mx_finalizado_l);
			//
			sem_post(&s_proceso_finalizado);
		}
		else{
			log_error(logger,"PCP || El estado recibido del PCB no es valido");
		}
		//	SACO EL SLEEP PERO HAY QUE ESTAR ATENTOS A VER SI DA ERROR
		//	Habilito la replanificacion de los procesos
		sleep(0.1);
		sem_post(&s_proceso_ejecutando);
	}
	return NULL;
}

bool esta_pcb_sjf(t_list* lista, pcb_t* pcb){
	for(int i=0; i<list_size(lista);i++){
		pcb_t* pcb_ = list_get(lista,i);
		if(pcb_->pid==pcb->pid)
			return true;
	}
	return false;
}

void* suspencion_sjf(void* pcb_){
	pcb_t* pcb = pcb_;
	log_trace(logger,"CONTADOR || VOY A DESPERTARME EN: %d || PCB: %d", (int) TIEMPO_BLOCK_MAX,pcb->pid);
	sleep(TIEMPO_BLOCK_MAX/1000);
	log_trace(logger,"CONTADOR || ME DESPERTE, POR LO TANTO, AVISO DE SUSPENDER || PCB: %d", pcb->pid);

	pthread_mutex_lock(&mx_susp_block_buffer_l);
	list_add(susp_block_buffer_l,pcb);
	pthread_mutex_unlock(&mx_susp_block_buffer_l);

	// LE AVISO AL PMP QUE HAY QUE SUSPENDER UN PROCESO
	sem_post(&s_proceso_susp);
	return NULL;
}

void* planificador_io_sjf(){
	pcb_t* pcb;
	while(1){
		sem_wait(&s_io_pendiente);
		log_trace(logger,"PLANIFICADOR IO || ENTRANDO A ENTRADA Y SALIDA");
		pcb= list_remove(block_l,0);
		log_trace(logger,"PLANIFICADOR IO || ENTRANDO A LA COLA DE ENTRADA Y SALIDA || PCB: %d",pcb->pid);
		block_t* block_pend = malloc(sizeof(block_t));
		block_pend->pcb=pcb;
		log_trace(logger,"PLANIFICADOR IO || DISPARANDO EL CONTADOR || PCB: %d",pcb->pid);
		int status=pthread_create(&block_pend->contador,NULL,suspencion_sjf,pcb);
		if(status<0){
			log_trace(logger,"------------------------ ERROR AL CREAR HILO CONTADOR ----------------------------------------");
		}
		pthread_detach(block_pend->contador);

		log_trace(logger,"PLANIFICADOR IO || DISPARE EL CONTADOR || PCB: %d",pcb->pid);
		pthread_mutex_lock(&mx_block_pend_l);
		list_add(block_pend_l,block_pend);
		pthread_mutex_unlock(&mx_block_pend_l);

		sem_post(&s_io);
		log_trace(logger,"PLANIFICADOR IO || PLANIFICACION DE IO CON EXITO || PCB: %d",pcb->pid);
	}
	return NULL;
}

void* io_sjf(){
	while(1){
		sem_wait(&s_io);
		log_trace(logger,"IO || ENTRANDO A IO");
		pthread_mutex_lock(&mx_block_pend_l);
		block_t* block_pend=list_remove(block_pend_l,0);
		pcb_t* pcb = block_pend->pcb;
		log_trace(logger,"IO || VOY A EJECUTAR IO || PCB: %d || TIEMPO: %d",pcb->pid,pcb->tiempo_block);
		pthread_mutex_unlock(&mx_block_pend_l);
		sleep(pcb->tiempo_block/1000);
		log_trace(logger,"IO || ME DESPERTE || PCB: %d",pcb->pid);
		if(!esta_pcb_sjf(susp_block_l,block_pend->pcb)){
			log_trace(logger,"IO || NO ESTA SUSPENDIDO, POR LO TANTO, MATO AL CONTADOR || PCB: %d",pcb->pid);
//			pthread_kill(block_pend->contador,SIGKILL);
			pthread_cancel(block_pend->contador);
			pthread_mutex_lock(&mx_unblock_l);
			list_add(unblock_l,pcb);
			pthread_mutex_unlock(&mx_unblock_l);
			sem_post(&s_proceso_ready);
		} else{
			log_trace(logger,"IO || Se DESBLOQUEO un proceso suspendido, ID:  %d", pcb->pid);
			pthread_mutex_lock(&mx_susp_block_l);
			remover_de_lista_sjf(susp_block_l,pcb);
			pthread_mutex_unlock(&mx_susp_block_l);

			// LO AÑADO A SUSPENDIDO READY
			pthread_mutex_lock(&mx_susp_ready_l);
			list_add(susp_ready_l,pcb);
			pthread_mutex_unlock(&mx_susp_ready_l);
			// TODO avisar a la memoria que se libere

			// LE AVISO QUE HAY UN PROCESO EN SUSP READY
			sem_post(&s_proceso_susp_ready);
		}
	}
	return NULL;
}

void interrumpir(){
	uint8_t intr = 25;
	send(socket_cpu_interrupt,&intr,sizeof(uint8_t),0);
}

void* agregar_a_ready_sjf(){
	pcb_t* pcb_interrumpido;
	while(1){
		pcb_interrumpido = NULL;
		sem_wait(&s_proceso_ready);
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
		pthread_mutex_lock(&mx_ready_l);
		list_add_sorted(ready_l, pcb, menor_estimacion);
		pthread_mutex_unlock(&mx_ready_l);
		log_info(logger,"PCP || PCP-READY Ingreso un proceso con PID: %d a la cola de READY", pcb->pid);
		if(proceso_ejecutando){
			log_trace(logger, "PCP || ------------------------------------VOY A INTERRUMPIR-------------------------------------------------------------");
			interrumpir();
			sem_wait(&s_interrupcion_atendida);
			pcb_interrumpido = list_remove(interrumpidos_l,0);
			list_add_sorted(ready_l,pcb_interrumpido,menor_estimacion);
		}
		mostrar_lista_ready_sjf(ready_l);
		sem_post(&s_cpu);
		if(pcb_interrumpido!=NULL)
		{
			sem_post(&s_cpu);
			sem_post(&s_espero_replanificacion);
		}
	}
	return NULL;
}

void* hablar_con_cpu(){
//	pthread_t hilo1;
	pthread_t hilo2;
	pthread_t hilo3;

//	pthread_create(&hilo1,NULL,interrupciones,NULL);
	pthread_create(&hilo3,NULL,enviar_a_ejecutar_sjf,NULL);
	pthread_create(&hilo2,NULL,devoluciones,NULL);
//	pthread_join(hilo1,NULL);
	pthread_join(hilo2,NULL);
	pthread_join(hilo3,NULL);
	return NULL;
}



void* sjf(){
	pthread_t hilo1;
	pthread_t hilo2;
	pthread_t hilo3;
	pthread_t hilo4;

	pthread_create(&hilo3,NULL,io_sjf,NULL);
	pthread_detach(hilo3);
	pthread_create(&hilo1,NULL,agregar_a_ready_sjf,NULL);
	pthread_detach(hilo1);
	pthread_create(&hilo2,NULL,hablar_con_cpu,NULL);
	pthread_detach(hilo2);
	pthread_create(&hilo4,NULL,planificador_io_sjf,NULL);

	pthread_join(hilo4,NULL);
	return NULL;

}
