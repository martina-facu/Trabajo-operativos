/*
 * var_glob.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef VAR_GLOB_H_
#define VAR_GLOB_H_


#include <commons/collections/list.h>
#include <semaphore.h>
#include <commons/log.h>

typedef struct{
	uint32_t pid;
	sem_t* s;
}comunicacion_t;


t_log* log;
t_log* PCP;
t_log* PMP;
t_log* PLP;
int grado_multiprogramacion;
double estimacion_inicial;
double alpha;
double TIEMPO_BLOCK_MAX;
char* algoritmo;
int server_fd;
int socket_cpu_cliente;
int socket_cpu_dispatch;
int socket_cpu_interrupt;
int socket_memoria;


#endif /* VAR_GLOB_H_ */
