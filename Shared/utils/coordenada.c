/*
 * coordenada.c
 *
 *  Created on: 4 jul. 2022
 *      Author: utnso
 */


#include "coordenada.h"

void enviar_coordenada(Coordenada_tabla* coordenada, uint32_t* valor_buscado, int conexion,uint8_t codigo_operacion){
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
	void* a_enviar = coordenada_serializar(coordenada,tamano_mensaje,codigo_operacion); // TODO: tener en cuenta que el enum es un int, revisar que pasa si se lo pasa a un uint8_t
	send(conexion, a_enviar, *tamano_mensaje, 0);

	recv(conexion, valor_buscado, sizeof(uint32_t), 0);
}

void *coordenada_armar_stream(Coordenada_tabla* coordenada) {
	int tamano = sizeof(Coordenada_tabla);
	void* stream = malloc(tamano);

	int desplazamiento = 0;

	memcpy(stream, &coordenada->id_tabla, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &coordenada->numero_entrada, sizeof(uint32_t));

	return stream;
}
;

void* coordenada_serializar(Coordenada_tabla* coordenada, uint32_t* tamano_mensaje, uint8_t codigo_operacion) {
	void* stream_coordenada = coordenada_armar_stream(coordenada);
	uint32_t tamano = sizeof(uint32_t) * 2;

	t_buffer* buffer = armar_buffer(tamano, stream_coordenada);
	t_paquete* paquete = empaquetar_buffer(buffer, codigo_operacion);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

	*tamano_mensaje = paquete->size;
	return a_enviar;
}

Coordenada_tabla* coordenada_deserializar(t_buffer* buffer) {
	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));
	void* stream = buffer->stream;

	memcpy(&(coordenada->id_tabla), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(coordenada->numero_entrada), stream, sizeof(uint32_t));

	return coordenada;
}

Coordenada_tabla* recibir_coordenada(int socket_cliente){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc (sizeof(t_paquete));
	t_buffer* buffer = paquete->buffer;

	//recibimos el cod de operacion
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	//recibimos el tamano del paquete
	recv(socket_cliente, &(buffer->size), sizeof(uint32_t), 0);

	//recibo buffer con coordenadas
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, 0);

	Coordenada_tabla* coordenada = coordenada_deserializar(buffer);

	return coordenada;
}
