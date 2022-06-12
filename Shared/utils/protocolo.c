#include "protocolo.h"

t_buffer* armar_buffer (uint32_t tamano_estructura,void* stream){
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = tamano_estructura;

	buffer->stream=stream;

	return buffer;
}

t_paquete* empaquetar_buffer(t_buffer* buffer, uint8_t codigo_operacion){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion= codigo_operacion;
	paquete->buffer= buffer;
	paquete->size = buffer->size+sizeof(uint8_t)+sizeof(uint32_t);

	return paquete;
}


void* serializar_paquete(t_paquete* paquete, void* a_enviar){
	t_buffer* buffer = paquete->buffer;

	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, buffer->stream, buffer->size);

	return a_enviar;
}

