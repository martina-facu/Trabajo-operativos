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

t_paquete *empaquetar_buffer(t_buffer* buffer){
	
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion= 0;
	paquete->buffer= buffer;
	paquete->size = buffer->size+sizeof(uint8_t)+sizeof(uint32_t);

	return paquete;
}

void* serializar_paquete(t_paquete* paquete){
	t_buffer* buffer = paquete->buffer;

	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, buffer->stream, buffer->size);

	return a_enviar;
}

