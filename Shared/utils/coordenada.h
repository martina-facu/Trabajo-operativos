#ifndef COORDENADA
#define COORDENADA

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include "protocolo.h"

typedef struct {
	uint32_t id_tabla;
	uint32_t numero_entrada;
} Coordenada_tabla;

void enviar_coordenada(Coordenada_tabla* coordenada, uint32_t* valor_buscado, int conexion);

void* coordenada_serializar(Coordenada_tabla* coordenada, uint32_t* tamano_mensaje, uint32_t codigo_operacion);

void *coordenada_armar_stream(Coordenada_tabla* coordenada);

Coordenada_tabla* coordenada_deserializar(t_buffer* buffer);

#endif
