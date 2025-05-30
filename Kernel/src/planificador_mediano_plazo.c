/*
 * planificador_mediano_plazo.c
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
#include "pcb.h"
#include "listas.h"
#include "semaforos.h"
#include "var_glob.h"
#include "conexion.h"
#include "planificador_mediano_plazo.h"

void* suspender_proceso(){
	while(1){
		sem_wait(&s_proceso_susp);
		pthread_mutex_lock(&mx_susp_block_buffer_l);
		pcb_t* pcb= list_remove(susp_block_buffer_l,0);
		pthread_mutex_unlock(&mx_susp_block_buffer_l);

		log_info(logger,"PMP || PMP-SUSPENSION Voy a suspender el proceso con PID: %d", pcb->pid);
		pthread_mutex_lock(&mx_susp_block_l);
		list_add(susp_block_l,pcb);
		pthread_mutex_unlock(&mx_susp_block_l);
		pcb->estado= SUSPENDIDO;

		uint8_t mensaje = SUSPENDER_PROCESO;
		pthread_mutex_lock(&mx_mensaje_memoria);
		send(socket_memoria,&mensaje,sizeof(uint8_t),0);

		log_trace(logger,"PMP || PMP-SUSPENSION Se envia mensaje de suspencion ");
		send(socket_memoria,&pcb->pid,sizeof(uint32_t),0);
		pthread_mutex_unlock(&mx_mensaje_memoria);
		sem_post(&s_grado_multiprogramacion);
//		sem_post(&s_susp);
	}
	return NULL;
}

void* avisar_proceso_susp_ready(){
	while(1){
		sem_wait(&s_proceso_susp_ready);
		sem_wait(&s_grado_multiprogramacion);
		pcb_t* pcb;
		pthread_mutex_lock(&mx_susp_ready_l);
		pcb= list_remove(susp_ready_l,0);
		pthread_mutex_unlock(&mx_susp_ready_l);
		//TODO PEDIR MEMORIA
		log_info(logger,"PMP || PMP-SUSPENCION-READY Se saca de suspencion al proceso con PID %d y se lo envia a READY",pcb->pid);
		pthread_mutex_lock(&mx_susp_readyM_l);
		list_add(susp_readyM_l,pcb);
		pthread_mutex_unlock(&mx_susp_readyM_l);

		sem_post(&s_proceso_ready);
		}
	return NULL;
}



void* administrador_mediano_plazo(){
	pthread_t hilo1;
	pthread_t hilo2;


	pthread_create(&hilo1,NULL,suspender_proceso,NULL);

	pthread_create(&hilo2,NULL,avisar_proceso_susp_ready,NULL);
	pthread_join(hilo1,NULL);
	pthread_join(hilo2,NULL);


	return NULL;
}
