/*
 * var_glob.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef VAR_GLOB_H_
#define VAR_GLOB_H_

#include "pcb.h"
#include <commons/collections/list.h>
#include <semaphore.h>
#include <commons/log.h>

typedef struct{
	uint32_t pid;
	sem_t* s;
}comunicacion_t;

typedef struct{
	pcb_t* pcb;
	pthread_t contador;
}block_t;

typedef struct
{
	int grado_multiprogramacion;
	double TIEMPO_BLOCK_MAX;
	char* algoritmo;
	double estimacion_inicial;
	double alpha;
	char* IP_CPU;
	char* IP_MEMORIA;
	char* PUERTO_ESCUCHA;
	char* PUERTO_MEMORIA;
	char* PUERTO_CPU_DISPATCH;
	char* PUERTO_CPU_INTERRUPT;
	char* IP_KERNEL;

}t_config_kernel;

/*
 * Listado de variables globales
*/
	t_log* logger;
//	t_log* logP;
//	t_log* PCP;
//	t_log* PMP;
//	t_log* PLP;
	//int grado_multiprogramacion;
	//double estimacion_inicial;
	double alpha;
	int proceso_ejecutando;
	double TIEMPO_BLOCK_MAX;
	//char* algoritmo;
	int server_fd;
	//int socket_cpu_cliente;
	int socket_cpu_dispatch;
	int socket_cpu_interrupt;
	int socket_memoria;

	t_config_kernel* configuracion;

#endif /* VAR_GLOB_H_ */
