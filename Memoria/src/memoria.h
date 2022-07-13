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
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <errno.h>
#include <conexion.h>
#include <logs.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h> //para funciones del swap como open
#include <pcb.h>



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

} t_tabla_paginas_segundo_nivel;

typedef struct{

	int pid; //Numero de proceso
	int tamanoProceso; //Tamanio de proceso o de instrucciones
	uint32_t* entrada_tabla_primer_nivel; //tabla_paginas_primer_nivel; //entrada a la tabla de paginas de primer nivel
	t_list* paginasDelProceso;
}t_proceso;

typedef struct{
	int pid;
	int posicion;
}ultimaSacada_t;


/*
 * Listado de variables globales
*/

	t_log* logger;
	t_config_memoria* config;

	//MEMORIA
	char* memoriaPrincipal;
	char* data;
	t_bitarray* marcosOcupadosPpal;
	int framesLibres;


	//paginacion -> Las tablas de paginas deben ser globales para todos los procesos

	t_list* tabla_paginas_primer_nivel_global;
	t_list* tabla_paginas_segundo_nivel_global;

	t_list* listaUltimaSacada; //algoritmos

	uint32_t indice_tabla_primer_nivel;
	uint32_t indice_tabla_primer_nivel_proceso;
	uint32_t indice_tabla_segundo_nivel;

	//Lista de procesos
	t_list* procesos;

/*
 * Prototipos de funciones
*/
	//CONFIGURACION
	t_config_memoria* cargarConfiguracion(char* configPath);
	t_config_memoria* crearConfigMemoria(void);

	//INICIAR MEMORIA
	int iniciar_memoria();
	void iniciar_tablas_paginas();
	void retardo_memoria();
	void inicializo_tabla_global_primer_nivel();
	void inicializo_tabla_global_segundo_nivel();
	void mostrar_tabla_primer_nivel_global(t_list* lista);
	void mostrar_tabla_segundo_nivel_global(t_list* lista);
	void iniciar_comunicacion();

	//FINALIZAR MEMORIA
	void liberar_memoria(int conexionKernel, int conexionCPU, t_log* logger, t_config* config);
	void liberar_conexion(int socket_cliente);

	//BITARRAY
	int bitsToBytes(int bits);
	char* asignarMemoriaBits(int bits);
	char* asignarMemoriaBytes(int bytes);
	void imprimir_bitarray(t_bitarray* marcosOcupadosPpal);
	void limpiar_posiciones(t_bitarray* marcosOcupados, t_proceso* proceso);

	//PAGINACION
	int obtener_y_ocupar_frame();
	void mostrar_tabla_primer_nivel(t_list* lista);
	int inicializo_tabla_primer_nivel_proceso(t_list* lista, t_proceso* proceso);
	int inicializo_tabla_segundo_nivel_proceso();
	void liberar_memoria_paginacion();

#endif /* MEMORIA_H_ */
