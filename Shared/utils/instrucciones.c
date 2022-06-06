#include "instrucciones.h"


uint8_t definirCodigo(char* id) {
	if (string_contains(id, "NO_OP")) {
		return 1;
	}
	if (string_contains(id, "I/O")) {
		return 2;
	}
	if (string_contains(id, "WRITE")) {
		return 3;
	}
	if (string_contains(id, "COPY")) {
		return 4;
	}
	if (string_contains(id, "READ")) {
		return 5;
	}
	if (string_contains(id, "EXIT")) {
		return 6;
	}else{
		return -1;
	}
}

int getCantidadParametros(uint8_t id) {
	switch (id) {
	case 1: case 2: case 5:
		return 1;
		break;
	case 3: case 4:
		return 2;
		break;
	case 6:
		return 0;
		break;
	default:
		printf("\n Se ingreso una operacion incorrecta");
		return -1;
		break;
	}
}

void mostrar_parametros(t_list* list){
	int aux = list_size(list);

	for(int i=0;i<aux;i++){
		int* parametro= list_get(list,i);
		printf(" %d ", *parametro);
	}
	printf("\n");
}
void mostrar_instrucciones(t_list* list){
	int aux = list_size(list);
	Instruccion* instruccion = malloc(sizeof(instruccion));
	char* nombre[6]={"NO_OP", "I/O", "WRITE", "COPY", "READ", "EXIT"};


	for(int i=0;i<aux;i++){
		instruccion= list_get(list,i);
		printf("\nEl codigo de la instruccion es: %d , nombre: %s \n cantidad de parametros: %d parametros: ", instruccion->id, nombre[(instruccion->id)-1], getCantidadParametros(instruccion->id));
		mostrar_parametros(instruccion->parametros);
	}
}

int calcular_espacio_instrucciones(t_list* instrucciones){
	int size = sizeof(uint32_t); // cantidad de instrucciones
	Instruccion* instruccion = malloc(sizeof(instruccion));
	int aux=list_size(instrucciones);

	for(int i=0;i<aux;i++){
		instruccion= list_get(instrucciones,i);
		size+= sizeof(uint8_t); //id
		size+=sizeof(uint32_t)*list_size(instruccion->parametros); //un int por parametro
	}
	return size;
}

void* armar_stream_instruccion(t_list* instrucciones){
	void* stream = malloc(calcular_espacio_instrucciones(instrucciones));
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
	return stream;
}

//t_buffer* intrucciones_armar_buffer (uint32_t tamano_estructura,void* stream){
//	t_buffer* buffer = malloc(sizeof(t_buffer));
//
//	buffer->size = tamano_estructura;
//
////	 = malloc(buffer->size);
////
////	llenar_stream_instruccion(instrucciones,stream);
//	buffer->stream=stream;
//
//	return buffer;
//}

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

//	free(&cant_instrucciones);
//	free(&cant_parametros);
}

