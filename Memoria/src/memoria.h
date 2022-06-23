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
#include <fcntl.h> //para funciones del swap como open

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


typedef struct{
uint32_t nroFrame;
uint32_t bPres; //bit de presencia
uint32_t bUso; //bit de uso
uint32_t bMod; // bit de modificado
} t_elementos_tabla;

//Represento una pagina como un entero

typedef struct {
int32_t idPagina;
} t_pagina;

//Represento un marco como un entero
typedef struct{
int32_t idFrame;
} t_frame;

//Lo utilizo para armar una lista y poder usar las funciones de las commons
typedef struct{
t_link_element *head;
t_elementos_tabla elements_count;
} t_tabla;

typedef struct{

int pid; //Numero de proceso
int tamanioInstrucciones; //Tamanio de proceso o de instrucciones
int dlPcb; //Direccion logica del pcb
t_list* paginas_primer_nivel; //tabla de paginas primer nivel (solo el numero de pagina)
t_tabla* paginas_segundo_nivel; //tabla de paginas ed segundo nivel
}t_tabla_pagina;


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

	char* memoriaPrincipal;
	int mem;
	char* data;
	t_bitarray* marcosOcupadosPpal;
	int cantidadDePaginasPorProceso;

/*
 * Prototipos de funciones
*/
	//CONFIGURACION
	t_config_memoria* cargarConfiguracion(char* configPath);
	t_config_memoria* crearConfigMemoria(void);

	//SERVIDOR
	int levantar_server(char* ipServer, char* portServer, t_log* logger);
	void aceptoYEvaluoConexion(int socketAnalizar);
	int validoYAceptoConexionKernel(int temporalAcceptedConnection);
	int validoYAceptoConexionCPU(int temporalAcceptedConnection);
	void iniciar_comunicacion();

	//INICIAR MEMORIA
	int iniciar_memoria();

	//BITARRAY
	int bitsToBytes(int bits);
	char* asignarMemoriaBits(int bits);
	char* asignarMemoriaBytes(int bytes);
	void imprimir_bitarray(t_bitarray* marcosOcupadosPpal);

	//Swap
	void iniciarSwap();
	int existe_archivo(char* path);
	void crear_archivo_swap(int pid);
	void eliminar_archivo_swap(int pid);
	int retardo_swap();

#endif /* MEMORIA_H_ */
