/*
 * main.h
 *
 *  Created on: 17 jul. 2022
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <errno.h>
#include <conexion.h>
#include <logs.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <semaphore.h>
#include <netdb.h>
#include <pcb.h>
#include "swap.h"



int socket_cpu;
int socket_kernel;
int socket_server;

void* memoria;
t_bitarray* bitMem;
t_log* logger;
char* PUERTO_ESCUCHA;
uint32_t TAM_MEMORIA;
uint32_t TAM_PAGINA;
uint32_t ENTRADAS_POR_TABLA;
int RETARDO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
uint32_t MARCOS_POR_PROCESO;
int RETARDO_SWAP;
char* pSwap;
int CANT_MARCOS;
sem_t s_swap;


#endif /* MAIN_H_ */
