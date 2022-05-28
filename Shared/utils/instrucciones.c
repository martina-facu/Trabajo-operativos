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

