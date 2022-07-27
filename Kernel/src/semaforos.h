/*
 * semaforos.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>


/*
 * Listado de variables globales
*/


	pthread_mutex_t mx_ready_l;
	pthread_mutex_t mx_new_l;
	pthread_mutex_t mx_newM_l;
	pthread_mutex_t mx_susp_ready_l;
	pthread_mutex_t mx_susp_readyM_l;
	pthread_mutex_t mx_block_l;
	pthread_mutex_t mx_unblock_l;
	pthread_mutex_t mx_finalizado_l;
	pthread_mutex_t mx_susp_block_l;
	pthread_mutex_t mx_susp_block_buffer_l;
	pthread_mutex_t mx_interrumpidos_l;
	pthread_mutex_t mx_comunicaciones_l;
	pthread_mutex_t mx_execute_l;
	pthread_mutex_t mx_proceso_ejecutando;
	pthread_mutex_t mx_mensaje_memoria;
	pthread_mutex_t mx_block_pend_l;

	sem_t s_proceso_ready;
	sem_t s_proceso_susp;
	sem_t s_cpu;
	sem_t s_proceso_finalizado;
	sem_t s_proceso_new;
	sem_t s_grado_multiprogramacion;
	sem_t s_proceso_susp_ready;
	sem_t s_interrupcion;
	sem_t s_interrupcion_atendida;
	sem_t s_proceso_ejecutando;
	sem_t s_susp;
	sem_t s_io_pendiente;
	sem_t s_io;



#endif /* SEMAFOROS_H_ */
