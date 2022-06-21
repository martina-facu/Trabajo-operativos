#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include "util.h"
//#include "utils.h"

//constantes archivo de config
#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define TAM_MEMORIA "TAM_MEMORIA"
#define TAM_PAGINA "TAM_PAGINA"
#define ENTRADAS_POR_TABLA "ENTRADAS_POR_TABLA"
#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"
#define MARCOS_POR_PROCESO "MARCOS_POR_PROCESO"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"
#define PATH_SWAP "PATH_SWAP"
//#define MEM_PPAL 0 para definir si la mem es ppal o virtual


//Estructuras

typedef struct{
char* listen_port;
int memory_size;
int page_size;
int table_input;
int memory_time_delay;
int quantity_frames_process;
int swap_time_delay;
char* path_swap;
char* replacement_algorithm;
}t_config_memoria;

//Variables

t_log* logger;
t_config_memoria* config;
char* path_config = "./memoria.config";
pthread_mutex_t mutexSuper;
char* memoriaPrincipal;
int mem;
char* data;
t_bitarray* marcosOcupadosPpal;
int cantidadDePaginasPorProceso;

//revisar esto
t_list* tablaDePaginas;


//Prototipos de funciones

//CONFIG
t_config_memoria* cargar_configuracion(char* ruta_del_config);

//LOGGER
t_log* iniciar_logger();
void mostrar_logger();

//MEMORIA
void liberear_memoria (int conexion, t_log* logger, t_config* config);
int iniciar_memoria();


//BITARRAY
int bitsToBytes(int bits);
char* asignarMemoriaBits(int bits);
char* asignarMemoriaBytes(int bytes);
void imprimir_bitarray(t_bitarray* marcosOcupadosPpal);


#endif /* CONFIGURACION_H_ */
