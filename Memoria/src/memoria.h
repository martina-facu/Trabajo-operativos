/*
 * memoria.h
 *
 *  Created on: 22 jun. 2022
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

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
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <conexion.h>

#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define TAM_MEMORIA "TAM_MEMORIA"
#define TAM_PAGINA "TAM_PAGINA"
#define ENTRADAS_POR_TABLA "ENTRADAS_POR_TABLA"
#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"
#define MARCOS_POR_PROCESO "MARCOS_POR_PROCESO"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"
#define PATH_SWAP "PATH_SWAP"
#define IP_MEMORIA "IP_MEMORIA"

#define CONCURRENT_CONNECTION 1
/*
 * Listado de estructuras
*/

typedef struct
{
	char* listen_port;
	int memory_size;
	int page_size;
	int table_input;
	int memory_time_delay;
	int quantity_frames_process;
	int swap_time_delay;
	char* path_swap;
	char* replacement_algorithm;
	char* memoryIP;
}t_config_memoria;

/*
 * Listado de variables globales
*/

	t_log* logger;
	t_config_memoria* config;
	fd_set  read_fd_set;
	fd_set  master_fd_set;
	int memorySocketServer;
	struct timeval 	tiempoSelect;

	int fdmax = -1;
	int fdmin = 201669;
	int memorySocketServer;

	int acceptedConecctionKernel;
	int acceptedConecctionCPU;


	int connectionsKernel = 0;
	int connectionsCPU = 0;

/*
 * Prototipos de funciones
*/

	t_config_memoria* cargarConfiguracion(char* configPath);
	int levantar_server(char* ipServer, char* portServer, t_log* logger);
	void aceptoYEvaluoConexion(int socketAnalizar);
	int validoYAceptoConexionKernel(int temporalAcceptedConnection);
	int validoYAceptoConexionCPU(int temporalAcceptedConnection);
	t_config_memoria* crearConfigMemoria(void);

#endif /* MEMORIA_H_ */
