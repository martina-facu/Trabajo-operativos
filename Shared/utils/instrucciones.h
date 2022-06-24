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
#include <commons/log.h>
#include "paquete.h"

/*
 * Estructuras
*/

	typedef struct {
		t_list* parametros;
		uint8_t id;
	} Instruccion;


/*
 * Prototipo de funciones
*/

	void mostrar_parametros(t_list* list, t_log* logger);
	void mostrar_instrucciones(t_list* list, t_log* logger);
	int calcular_espacio_instrucciones(t_list* instrucciones);
	uint8_t definirCodigo(char* id);
	int getCantidadParametros(uint8_t id);
	void deserializar_instrucciones(t_buffer* buffer,t_list* instrucciones);
	void* armar_stream_instruccion(t_list* instrucciones);
	t_list* deserializar_paquete_instrucciones(int cliente, uint32_t* tamano_proceso);


#endif
