#ifndef CPU
#define CPU

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <conexion.h>
#include <pcb.h>
#include <paquete.h>
#include <logs.h>
#include <pthread.h>
#include <errno.h>

#define SERVER_DISPATCH "SERVER DISPATCH"
#define SERVER_INTERRUPT "SERVER INTERRUPT"
#define CPU_IP "IP_CPU"
// Thread stack size 64KB
#define THREAD_STACK_SIZE 65536
// maximum concurrent connections
#define CONCURRENT_CONNECTION 1


/*
 * Listado de variables globales
*/

	//	t_log* logger;
	//	t_config* config;
	int fdDispatch;
	// thread identifier
	pthread_t thread_id;
	struct timeval 	tiempoSelect;
	fd_set read_fd_set;	//	conjunto temporal de fd para select
//	int fdmax;
//	int fdmin;
//	int connectionsDispatch;
//	int connectionsInterrupt;
//	int acceptedConecctionDispatch;
//	int acceptedConecctionInterrupt;
	Pcb* pcb;
	fd_set master_fd_set; 		// 	conjunto maestro de descriptores de fichero
	bool devolver_pcb;		//	Si es True significa que recibi una interrupcion
	bool recibiPCB;			//	Si es FALSE significa que todavia no recibi ningun PCB si es TRUE ya recibi PCB a procesar desde el Kernel

/*
 * Prototipo de funciones
*/



	int levantar_conexion_memoria(char* ipServer, char* portServer, t_log* logger, uint32_t* cantidad_entradas,uint32_t* tamano_pagina);

	int levantar_server(char* ipServer, char* portServer, t_log* logger, char* sTipo);
//	int levantar_server(t_config* config, t_log* logger, char* sTipo);

	Pcb* obtener_pcb(int cliente);
	bool execute(Instruccion* instruccion,int dormir, Pcb* pcb);
//	bool execute(Instruccion* instruccion,t_config* config, Pcb* pcb);

#endif
