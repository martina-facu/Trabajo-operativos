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


t_list* tabla;

} t_tabla_paginas_segundo_nivel;


typedef struct {

	int nroFrame;
	uint32_t bPres; //bit de presencia
	uint32_t bUso; //bit de uso
	uint32_t bMod; // bit de modificado

}t_entradas_segundo_nivel;

typedef struct{

	uint32_t pid; //Numero de proceso
	uint32_t tamanoProceso; //Tamanio de proceso o de instrucciones
	uint32_t entrada_tabla_primer_nivel;  //entrada a la tabla de paginas de primer nivel
	t_list* paginasDelProceso;
	uint32_t punteroAlgoritmo;
	uint32_t contador;
}t_proceso;

typedef struct{
	uint32_t pid;
	uint32_t posicion;
}ultimaSacada_t;


typedef struct{

	uint32_t pid;
	uint32_t indice_tabla_primer_nivel;

} Coordenada_tabla_cpu;


typedef struct{

	uint32_t direccionFisica;
	int tipoDeOperacion; //LECTURA 0 O ESCRITURA 1
} EstructuraDeOperacion;


/*
 * Listado de variables globales
*/

	t_log* logger;
	t_config_memoria* config;

	//MEMORIA
	void* memoriaPrincipal;
	void* data;
	t_bitarray* marcosOcupadosPpal;
	int framesLibres;


	//SERVER
	int acceptedConecctionKernel;
	int acceptedConecctionCPU;

	//Para responder el marco en el segundo mensaje a CPU
	uint32_t valorMarco;

	t_list* tabla_paginas_primer_nivel_global;
	t_list* tabla_paginas_segundo_nivel_global;

	t_list* listaUltimaSacada; //algoritmos

	uint32_t indice_tabla_primer_nivel;
	uint32_t indice_tabla_primer_nivel_proceso;
	uint32_t indice_tabla_segundo_nivel;

	//CONFIG
	int tamanoPagina;
	int entradasPorTabla;
	char* pSwap;
	int tamanoMemoria;
	int swapDelay;
	int memoryDelay;
	int marcosPorProceso;
	char* algorithm;

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
	void mostrar_lista_procesos(t_list* lista);
	int cantidad_de_entrada_primer_nivel(int tamanoProceso, int entradasPorTabla, int tamanoPagina);
	int cantidad_de_paginas_del_proceso(int tamanioProceso, int tamanoPagina);

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
	uint32_t inicializo_tabla_primer_nivel_proceso(t_proceso* proceso, int cantEntradas, int cantPag);
	int inicializo_tabla_segundo_nivel_proceso(t_proceso* proceso, int entradasPorTabla);
	void liberar_memoria_paginacion();
	bool esta_en_memoria(t_tabla_paginas_segundo_nivel* tabla, uint32_t indice_tabla_segundo_nivel);
	bool ordenar(void* entrada1, void* entrada2);
	void mostrar_vector(uint32_t* vector_primer_nivel);

#endif /* MEMORIA_H_ */
