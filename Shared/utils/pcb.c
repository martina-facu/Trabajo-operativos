/*
 * pcb.c
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */


#include <ctype.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include "pcb.h"

//pcb_t *pcb_create(uint32_t tamano, t_list* instrucciones,uint32_t pid, double estimacion_inicial,
//		uint32_t tabla_paginas, uint32_t estimado_rafaga)
pcb_t *pcb_create(uint32_t tamano, t_list* instrucciones,uint32_t pid, double estimacion_inicial,
		uint32_t tabla_paginas)
{
	pcb_t *pcb = malloc(sizeof(pcb_t));
	pcb->pid = pid;
	pcb->tamano = tamano;
	pcb->program_counter = 0;
	pcb->estimado_rafaga = estimacion_inicial;
	pcb->tiempo_ejecucion=0;
//	pcb->estimado_rafaga = estimacion_inicial;
	pcb->estado = INICIADO;
	pcb->tiempo_block = 0;
	pcb->tabla_paginas = tabla_paginas;
	pcb->instrucciones = instrucciones;
	return pcb;
}

uint32_t pcb_calcular_espacio(pcb_t* pcb)
{
	uint32_t tamano_instrucciones = calcular_espacio_instrucciones(pcb->instrucciones);
	return  tamano_instrucciones + sizeof(uint32_t) * 8; // PID, TAMANO, PC, ESTIMACION, estado, tiempo_bloqueo, TABLA
}

void *pcb_armar_stream(pcb_t *pcb)
{
	int tamano_pcb = pcb_calcular_espacio(pcb);
	void* stream = malloc(tamano_pcb);

	int desplazamiento = 0;

	memcpy(stream, &pcb->pid, sizeof(uint32_t)); // PID
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tamano, sizeof(uint32_t)); // TAMANO
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->program_counter, sizeof(uint32_t)); // PC
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->estimado_rafaga, sizeof(uint32_t)); // ESTIMACION
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->estado, sizeof(uint32_t)); // ESTADO
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tiempo_block, sizeof(uint32_t)); // TIEMPO BLOCK, COMENTARIO(NACHO): NO SE SI ES NECESARIO PASAR ESTE ATRIBUTO PERO BUENO, NO IMPORTA
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tabla_paginas, sizeof(uint32_t)); // TABLA PAGINA
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tiempo_ejecucion, sizeof(uint32_t)); // TABLA PAGINA
	desplazamiento += sizeof(uint32_t);

	void* stream_instrucciones = (armar_stream_instruccion(pcb->instrucciones));
	memcpy(stream + desplazamiento,stream_instrucciones,calcular_espacio_instrucciones(pcb->instrucciones)); // INSTRUCCIONES

	free(stream_instrucciones);
	return stream;
}


void* pcb_serializar(pcb_t* pcb, uint32_t* tamano_mensaje, uint8_t codigo_operacion)
{
	void* stream_pcb = pcb_armar_stream(pcb);
	uint32_t tamano_pcb = pcb_calcular_espacio(pcb);

	t_buffer* buffer = armar_buffer(tamano_pcb, stream_pcb);
	t_paquete* paquete = empaquetar_buffer(buffer, codigo_operacion);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

	*tamano_mensaje = paquete->size;

	free(stream_pcb);
	free(buffer);
	free(paquete);
	return a_enviar;
}

void pcb_deserializar(t_buffer* buffer, pcb_t* pcb)
{
//	pcb_t* pcb = malloc(sizeof(pcb_t));
	void* stream = buffer->stream;

	memcpy(&(pcb->pid), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->tamano), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->program_counter), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->estimado_rafaga), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->estado), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->tiempo_block), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->tabla_paginas), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->tiempo_ejecucion), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);


	buffer->stream = stream;
	t_list* instrucciones = list_create();
	deserializar_instrucciones(buffer, instrucciones);
	pcb->instrucciones = instrucciones;
//	return pcb;
}


void pcb_mostrar(pcb_t* pcb, t_log* logger)
{
	char* estados[5]={"INICIADO", "BLOQUEADO", "FINALIZADO","SUSPENDIDO","INTERRUMPIDO"};
//	printf("\n\nINFORMACION PCB:\n");
	log_trace(logger, "------------------- INFORMACION PCB -------------------");
//	printf("PID: %d\n", pcb->pid);
	log_trace(logger, "PCB || PID: %d", pcb->pid);
//	printf("TAMANO: %d\n", pcb->tamano);
	log_trace(logger, "PCB || TAMANO: %d", pcb->tamano);
//	printf("PC: %d\n", pcb->program_counter);
	log_trace(logger, "PCB || PC: %d", pcb->program_counter);
//	printf("ESTIMADO_RAFAGA: %d\n", pcb->estimado_rafaga);
	log_trace(logger, "PCB || ESTIMADO_RAFAGA: %d", pcb->estimado_rafaga);
//	printf("ESTADO: %s\n", estados[pcb->estado]);
	log_trace(logger, "PCB || ESTADO: %s", estados[pcb->estado]);
//	printf("TIEMPO BLOQUEO: %d\n", pcb->tiempo_block);
	log_trace(logger, "PCB || TIEMPO BLOQUEO: %d", pcb->tiempo_block);
	mostrar_instrucciones(pcb->instrucciones, logger);
	log_trace(logger, "------------------- FIN INFORMACION PCB -------------------");
}

void recibirPCB(int socket, pcb_t* pcb)
{

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;
	//recibimos el codigo del tipo de mensaje que nos llega
	recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	//recibo el tamaño del paquete
	recv(socket, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con el pcb
	buffer->stream = malloc(buffer->size);
	recv(socket, buffer->stream, buffer->size, 0);

	pcb_deserializar(buffer,pcb);

	free(buffer);
	free(paquete);
//	return pcb;
}


void pcb_liberar (pcb_t* pcb){
	t_list* instrucciones = pcb->instrucciones;
	destruir_lista_instrucciones(instrucciones);
	free(pcb);
}





