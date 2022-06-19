#include "./pcb.h"

Pcb *pcb_create(uint32_t pid, uint32_t tamano, uint32_t tabla_paginas,
		uint32_t estimado_rafaga, t_list* instrucciones) {
	Pcb *pcb = (Pcb *) malloc(sizeof(Pcb));
	pcb->pid = pid;
	pcb->tamano = tamano;
	pcb->program_counter = 0;
	pcb->estimado_rafaga = estimado_rafaga;
	pcb->estado = INICIADO;
	pcb->tiempo_bloqueo = 0;
	pcb->tabla_paginas = tabla_paginas;
	pcb->instrucciones = instrucciones;
	return pcb;
}
;

uint32_t pcb_calcular_espacio(Pcb* pcb) {
	uint32_t tamano_instrucciones = calcular_espacio_instrucciones(
			pcb->instrucciones);
	return tamano_instrucciones + sizeof(uint32_t) * 7; // PDI, TAMANO, PC, ESTIMACION, estado, tiempo_bloqueo, tabla de paginas
}
;

void *pcb_armar_stream(Pcb *pcb) {
	int tamano_pcb = pcb_calcular_espacio(pcb);
	void* stream = malloc(tamano_pcb);

	int desplazamiento = 0;

	memcpy(stream, &pcb->pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tamano, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->program_counter, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->estimado_rafaga, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->estado, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tiempo_bloqueo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pcb->tabla_paginas, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento,
			armar_stream_instruccion(pcb->instrucciones),
			calcular_espacio_instrucciones(pcb->instrucciones));

	return stream;
}
;

void* pcb_serializar(Pcb* pcb, uint32_t* tamano_mensaje,
		uint32_t codigo_operacion) {
	void* stream_pcb = pcb_armar_stream(pcb);
	uint32_t tamano_pcb = pcb_calcular_espacio(pcb);

	t_buffer* buffer = armar_buffer(tamano_pcb, stream_pcb);
	t_paquete* paquete = empaquetar_buffer(buffer, codigo_operacion);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

	*tamano_mensaje = paquete->size;

	return a_enviar;
}

Pcb* pcb_deserializar(t_buffer* buffer) {
	Pcb* pcb = malloc(sizeof(Pcb));
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

	memcpy(&(pcb->tiempo_bloqueo), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->tabla_paginas), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	buffer->stream = stream;
	t_list* instrucciones = list_create();
	deserializar_instrucciones(buffer, instrucciones);
	pcb->instrucciones = instrucciones;
	return pcb;
}
;

void pcb_mostrar(Pcb* pcb) {
	printf("\n\nINFORMACION PCB:\n");
	printf("PID: %d\n", pcb->pid);
	printf("TAMANO: %d\n", pcb->tamano);
	printf("PC: %d\n", pcb->program_counter);
	printf("ESTIMADO_RAFAGA: %d\n", pcb->estimado_rafaga);
	printf("ESTADO: %d\n", pcb->estado);
	printf("TIEMPO BLOQUEO: %d\n", pcb->tiempo_bloqueo);
	printf("ID TABLA DE PAGINAS: %d\n", pcb->tabla_paginas);
	mostrar_instrucciones(pcb->instrucciones);
}
;
