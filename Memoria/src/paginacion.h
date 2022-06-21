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

//Estructuras

typedef struct{
uint32_t nroFrame;
uint32_t bPres; //bit de presencia
uint32_t bUso; //bit de uso
uint32_t bMod // bit de modificado
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

//Seria un struct de los elementos la tabla de segundo nivel

//Variables
pthread_mutex_t mutexListaTablas;
pthread_mutex_t mutexBitmapMP;


//Prototipos de funciones

//INICIALIZAR PROCESO
t_tabla_pagina* crear_tabla_de_paginas(int pid, int size);
int iniciar_paginacion(int pid, char* instrucciones, int tamanioTotal);



#endif /* CONFIGURACION_H_ */
