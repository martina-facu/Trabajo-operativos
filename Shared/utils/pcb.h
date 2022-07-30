/*
 * pcb.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include "protocolo.h"
#include "instrucciones.h"
#include <stdint.h>

/*
 * Estructuras
*/


	typedef enum {INICIADO, BLOQUEADO, FINALIZADO, SUSPENDIDO, INTERRUMPIDO} ESTADO;

	typedef struct {
	   uint32_t pid;
	   uint32_t tamano;
	   uint32_t program_counter;
	   uint32_t estimado_rafaga;
	   ESTADO estado;
	   uint32_t tiempo_ejecucion;
	   uint32_t tiempo_block;
	   uint32_t tabla_paginas;
	   t_list* instrucciones;
	} pcb_t;




/*
 * Prototipo de funciones
*/


	pcb_t *pcb_create(uint32_t tamano, t_list* instrucciones,uint32_t pid, double estimacion_inicial,
			uint32_t tabla_paginas);
	void *pcb_armar_stream(pcb_t *pcb);
	pcb_t *pcb_deserializar(t_buffer* buffer, t_log* logger);
	void pcb_mostrar(pcb_t* pcb, t_log* logger);
	uint32_t pcb_calcular_espacio(pcb_t* pcb);
	void* pcb_serializar(pcb_t* pcb, uint32_t* tamano_mensaje, uint8_t codigo_operacion);
	pcb_t* recibirPCB(int socket, t_log* logger);

#endif /* PCB_H_ */
