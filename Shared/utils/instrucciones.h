#ifndef INSTRUCCIONES
#define INSTRUCCIONES

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include "paquete.h"

typedef struct {
	t_list* parametros;
	uint8_t id;
} Instruccion;

typedef enum {NO_VALIDO, NO_OP, I_O,WRITE, COPY, READ, EXIT} INSTRUCCION;


void mostrar_parametros(t_list* list);

void mostrar_instrucciones(t_list* list);

int calcular_espacio_instrucciones(t_list* instrucciones);

uint8_t definirCodigo(char* id);

int getCantidadParametros(uint8_t id);

void deserializar_instrucciones(t_buffer* buffer,t_list* instrucciones);

void* armar_stream_instruccion(t_list* instrucciones);

void destruir_lista_instrucciones(t_list* instrucciones);

#endif
