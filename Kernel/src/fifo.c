/*
 * fifo.c
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */


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
#include "fifo.h"

// PCP: PLANIFICADOR CORTO PLAZO
// PLP: PLANIFICADOR LARGO PLAZO
// PMP: PLANIFICADOR MEDIANO PLAZO

void mostrar_lista_ready(t_list* lista){
	int i;
	int lista_size= list_size(lista);
	pcb_t* pcb= malloc(sizeof(pcb_t));
	for(i=0;i<lista_size;i++){
		pcb= list_get(lista,i);
		log_trace(PCP,"KERNEL-CPU-PCB  PCB = %d, prioridad= %d \n", pcb->pid,i);
//		printf("PCB = %d, prioridad= %d \n", pcb->pid,i);
	}
}

//--------------------------------------------------------------------------------------//
//FUNCION: 							AGREGAR A READY
//RESPONSABILIDAD: 	AGREGAR UN PROCESO A READY EN MODO FIFO
//ENTRADAS: 		AVISO QUE HAY QUE AGREGAR UN PROCESO A READY
//SALIDAS:			AVISO QUE HAY UN PROCESO PARA EJECUTAR
//--------------------------------------------------------------------------------------//

void* agregar_a_ready(){ // REVISAR(no estoy seguro que sea un problema): SI SE BLOQUEO EL HILO EN NEW Y, MIENTRAS ESTA BLOQUEADO, SE AGREGA UN NUEVO PROCESO EN SUSP READY, VA A SACAR EL DE NEW PRIMERO
	while(1){
		// ESPERO QUE HAYA UN PROCESO PARA CARGAR A READY
		sem_wait(&s_proceso_ready);
		pcb_t* pcb;
		// ME FIJO QUE PROCESO VOY A AGREGAR DEPENDIENDO DE LA PRIORIDAD
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
		log_info(PCP,"KERNEL Se esta pasando un pcb a ready");
		// LO AGREGO
		pthread_mutex_lock(&mx_ready_l);
		list_add(ready_l,pcb);
		pthread_mutex_unlock(&mx_ready_l);
		mostrar_lista_ready(ready_l);
		// AVISO QUE HAY UN PROCESO EN READY PARA EJECUTAR
		sem_post(&s_cpu);
	}
	return NULL;
}

//--------------------------------------------------------------------------------------//
//FUNCION: 							ENVIAR A EJECUTAR
//RESPONSABILIDAD:  ENVIAR A EJECUTAR UN PROCESO A LA CPU
//ENTRADAS: 		AVISO QUE HAY QUE ENVIAR UN PROCESO A EJECUTAR Y QUE PUEDO ENVIAR A EJECUTAR
//SALIDAS:			ENVIAR PROCESO A EJECUTAR
//--------------------------------------------------------------------------------------//

void* enviar_a_ejecutar()  {
	while(1)
	{
		// ESPERO QUE HAYA PROCESOS PARA EJECUTAR Y QUE NO SE ESTE EJECUTANDO NINGUNO
		sem_wait(&s_cpu);
		sem_wait(&s_proceso_ejecutando);

		// SACO EL PCB PARA EJECUTAR
		pthread_mutex_lock(&mx_ready_l);
		pcb_t* pcb= list_remove(ready_l,0);
		pthread_mutex_unlock(&mx_ready_l);

		uint32_t espacio;
		log_trace(PCP,"KERNEL-CPU-PCB Se va a mandar a ejecutar un proceso %d", pcb->pid);

		// ENVIO EL PROCESO A EJECUTAR
		void* a_enviar= pcb_serializar(pcb,&espacio,0);
		send(socket_cpu_dispatch,a_enviar,espacio,0);
	}
	return NULL;
}

void remover_de_lista(t_list* lista,pcb_t* pcb){
	for(int i=0;i<list_size(lista);i++){
		pcb_t* pcb_aux= list_get(lista,i);
		if(pcb_aux->pid == pcb->pid){
			list_remove(lista,i);
			return;
		}
	}
}
//--------------------------------------------------------------------------------------//
//FUNCION: 							BLOQUEAR PROCESO
//RESPONSABILIDAD: 	BLOQUEAR AL PROCESO POR N MILISEGUNDOS SABIENDO SI HAY QUE SUSPENDER O NO
//ENTRADAS: 		PCB A BLOQUEAR
//SALIDAS:			AVISAR QUE EL PROCESO SE DESPERTO
//--------------------------------------------------------------------------------------//
void* bloquear_proceso(void* pcb_){
	pcb_t* pcb= pcb_;
	// EVALUO SI HAY QUE SUSPENDER
	if(pcb->tiempo_block > configuracion->TIEMPO_BLOCK_MAX)
	{ // CONDICION DE SUSPENSION
		log_info(PCP,"KERNEL-CPU-PCB Se va a suspender un proceso por %d, ID: %d", pcb->tiempo_block,pcb->pid);
		// LO SACO DE LISTAS DE BLOQUEADOS
		pthread_mutex_lock(&mx_block_l);
		remover_de_lista(block_l,pcb);
		pthread_mutex_unlock(&mx_block_l);
		// LO AÑADO A UN BUFFER PARA QUE EL PMP SE HAGA CARGO DE PASARLO AL ESTADO BLOQUEADO SUSPENDIDO
		pthread_mutex_lock(&mx_susp_block_buffer_l);
		list_add(susp_block_buffer_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_buffer_l);

		// LE AVISO AL PMP QUE HAY QUE SUSPENDER UN PROCESO
		sem_post(&s_proceso_susp);
		// ESPERO QUE LO HAYA PUESO BLOQUEADO SUSPENDIDO
		sem_wait(&s_susp);
		
		uint32_t mensaje = SUSPENDER_PROCESO;
		send(socket_memoria, &mensaje, sizeof(uint32_t), 0);
		//Envio el PID del proceso a suspender
		mensaje = pcb->pid;
		send(socket_memoria,&mensaje, sizeof(uint32_t),0);

		// LO SUSPENDO POR N MILISEGUNDOS
		usleep(pcb->tiempo_block);

		// SE DESPERTO Y LO SACO DE SUSP BLOQUEADO
		log_info(PCP,"KERNEL-CPU-PCB  Se DESBLOQUEO un proceso suspendido, ID:  %d", pcb->pid);
		pthread_mutex_lock(&mx_susp_block_l);
		remover_de_lista(susp_block_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_l);

		// LO AÑADO A SUSPENDIDO READY
		pthread_mutex_lock(&mx_susp_ready_l);
		list_add(susp_ready_l,pcb);
		pthread_mutex_unlock(&mx_susp_ready_l);
		// TODO avisar a la memoria que se libere

		// LE AVISO QUE HAY UN PROCESO EN SUSP READY
		sem_post(&s_proceso_susp_ready);
	}
	else{
		log_info(PCP,"KERNEL-CPU-PCB Se va a bloquear un proceso por ID: %d", pcb->pid);
		//DUERMO EL PROCESO POR N MILISEGUNDOS
		usleep(pcb->tiempo_block);

		log_info(logP,"Se DESBLOQUEO un proceso, ID: %d", pcb->pid);

		// SE DESPERTO Y LO SACO DE LA LISTA DE BLOQUEADOS
		pthread_mutex_lock(&mx_block_l);
		remover_de_lista(block_l,pcb);
		pthread_mutex_unlock(&mx_block_l);

		// LO AÑADO A LA LISTA DE DESBLOQUEADOS QUE FUNCIONA COMO BUFFER
		// OBSERVACION: ESTO NO ES NECESARIO, PODRIAMOS CAMBIAR EL ESTADO A DESBLOQUEADO Y BUSCAR CUANDO
		// SE QUIERA PASAR A READY DE SACAR LOS QUE ESTAN DESBLOQUEADOS PERO BUENO, YA ESTA ASI
		pthread_mutex_lock(&mx_unblock_l);
		list_add(unblock_l,pcb);
		pthread_mutex_unlock(&mx_unblock_l);

		// AVISO QUE HAY UN PROCESO DESBLOQUEADO PARA PASAR A READY
		sem_post(&s_proceso_ready);
	}
	return NULL;
}

pcb_t* recibir_paquete_pcb()
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));
	recv(socket_cpu_dispatch,&paquete->codigo_operacion,sizeof(uint8_t),0);
	recv(socket_cpu_dispatch,&buffer->size,sizeof(uint32_t),0);
	buffer->stream = malloc(buffer->size);
	recv(socket_cpu_dispatch,buffer->stream,buffer->size,0);
	pcb_t* pcb = pcb_deserializar(buffer);
	pcb_mostrar(pcb, PCP);
	log_info(PCP,"KERNEL-CPU-PCB Recibi un proceso ID: %d ",pcb->pid);
	return pcb;
}

//--------------------------------------------------------------------------------------//
//FUNCION: 							RECIBIR PROCESO DE CPU
//RESPONSABILIDAD: 	RECIBIR PROCESO DE LA CPU Y EVALUAR QUE TIENE QUE HACER, SI BLOQUEAR O FINALIZAR
//ENTRADAS: 		PCB
//SALIDAS:			SI EL PROCESO FINALIZA, AVISAR QUE HAY UN PROCESO FINALIZADO
//					SI EL PROCESO SE TIENE QUE BLOQUEAR, AVISAR QUE HAY UN PROCESO A BLOQUEAR
//					AVISAR QUE NO HAY NINGUN PROCESO EJECUTANDO
//--------------------------------------------------------------------------------------//
void* recibir_proceso_de_cpu(){
	while(1)
	{
		pcb_t* pcb = recibir_paquete_pcb();
		log_info(PCP,"KERNEL-CPU-PCB Recibi PCB desde la CPU");
		if(pcb->estado == FINALIZADO){ // SE PODRIA MODELAR CON UN SWITCH
			// AGREGARLO A FINALIZADO QUE ES UN BUFFER ENTRE PLP Y PCP
			pthread_mutex_lock(&mx_finalizado_l);
			list_add(finalizado_l,pcb);
			pthread_mutex_unlock(&mx_finalizado_l);
			// AVISAR QUE HAY UN PROCESO FINALIZADO
			log_info(PCP,"KERNEL-CPU-PCB Libero semaforo para finalizar proceso con ID: %d ",pcb->pid);
			sem_post(&s_proceso_finalizado);

		}
		else if(pcb->estado==BLOQUEADO){
			// AGREGARLO A LA LISTA DE BLOQUEADOS
			pthread_mutex_lock(&mx_block_l);
			list_add(block_l,pcb);
			pthread_mutex_unlock(&mx_block_l);

			// CREAR EL HILO QUE SE VA A BLOQUEAR UN EL PCB ADENTRO
			pthread_t hilo_bloqueante;
			pthread_create(&hilo_bloqueante,NULL,bloquear_proceso,pcb);
			pthread_detach(hilo_bloqueante);
		}
		else{
			log_error(PCP,"KERNEL-CPU-PCB Error el estado del proceso es desconocido");
//			printf("ocurrio un error al recibir al proceso: funcion: recibir_proceso_de_cpu \n");
			return NULL;
		}
		// AVISAR QUE NO HAY NINGUN PROCESO EJECUTANDO
		sem_post(&s_proceso_ejecutando);
	}
	return NULL;
}

void* fifo(){
	pthread_t hilo1;
	pthread_t hilo2;
	pthread_t hilo3;

	pthread_create(&hilo1,NULL,agregar_a_ready,NULL);
	pthread_create(&hilo2,NULL,enviar_a_ejecutar,NULL);
	pthread_create(&hilo3,NULL,recibir_proceso_de_cpu,NULL);
	pthread_join(hilo1,NULL);
	pthread_join(hilo2,NULL);
	pthread_join(hilo3,NULL);
	return NULL;
}
