#include "protocolo.h"

void llenar_stream_instruccion(t_list* instrucciones,void* stream){
	int desplazamiento=0;
	Instruccion* instruccion = malloc(sizeof(instruccion));
	uint32_t aux=list_size(instrucciones);
	memcpy(stream+desplazamiento,&aux,sizeof(uint32_t)); // CANT DE INSTRUCCIONES
	desplazamiento+= sizeof(uint32_t);
	uint8_t aux2;
	int* prm;
	for(int i=0;i<list_size(instrucciones);i++){
		instruccion= list_get(instrucciones,i);
		aux2= instruccion->id;
		memcpy(stream+desplazamiento,&aux2,sizeof(uint8_t)); // ID
		desplazamiento+=sizeof(uint8_t);
		int cant_prm = list_size(instruccion->parametros);
		for(int i=0;i<cant_prm;i++){							// PARAMETROS
			prm= list_get(instruccion->parametros,i);
			memcpy(stream+desplazamiento,prm,sizeof(uint32_t));
			desplazamiento+=sizeof(uint32_t);
		}
	}
}

void deserializar_instrucciones(t_buffer* buffer,t_list* instrucciones){
	void* stream = buffer->stream;
	uint32_t* parametro;
	uint32_t cant_instrucciones;
	int cant_parametros = 0;
	uint8_t id;
	memcpy(&cant_instrucciones,stream,sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	for(int i=0;i<cant_instrucciones;i++){
		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->parametros= list_create();
		memcpy(&id,stream,sizeof(uint8_t));
		stream+=sizeof(uint8_t);
		instruccion->id = id;
		cant_parametros= getCantidadParametros(instruccion->id);
		for(int i=0;i<cant_parametros;i++){
			parametro= malloc(sizeof(uint32_t));
			memcpy(parametro,stream,sizeof(uint32_t));
			stream+=sizeof(uint32_t);
			list_add(instruccion->parametros,parametro);
		}
		list_add(instrucciones,instruccion);
	}

	free(cant_instrucciones);
	free(cant_parametros);
}

t_paquete* empaquetar_instrucciones(t_list* instrucciones){ // TODO : hacer que la fucion reciba un buffer
	//Estaria en la funcion armar_buffer_instrucion ---------------->
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = calcular_espacio_instrucciones(instrucciones);

	void* stream = malloc(buffer->size);

	llenar_stream_instruccion(instrucciones,stream);
	buffer->stream=stream;
	//-------------------------------------------------->
	
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion= 0;
	paquete->buffer= buffer;
	paquete->size = buffer->size+sizeof(uint8_t)+sizeof(uint32_t);

	return paquete;
}

void* serializar_instrucciones(t_paquete* paquete){ // serializar_paquete
	t_buffer* buffer = paquete->buffer;

	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
	//void* a_enviar = malloc(2*sizeof(uint8_t) + buffer->size + sizeof(uint32_t));
	int offset = 0;

	//memcpy(a_enviar + offset, tamano_proceso, sizeof(uint8_t));
	//offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, buffer->stream, buffer->size);

	return a_enviar;
}

