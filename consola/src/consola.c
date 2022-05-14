#include <stdio.h>
#include <stdlib.h>
// #include "cliente.h"
#include <sys/stat.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>


// void exit_gracefully(int return_nr) {
//   exit(return_nr);
// }

//------------------------------------------Estructuras ----------------------------------------------------------
typedef struct {
	t_list* parametros;
	int id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} Instruccion;

//typedef struct {
//	uint32_t size;
//	void* stream;
//} t_buffer;

//typedef struct {
//	uint8_t codigo_operacion;
//	t_buffer* buffer;
//} t_paquete;

//----------------------------------Funciones---------------------------------------------
int definirCodigo(char* id) {
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

int getCantidadParametros(int id) {
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

void mostrar_instrucciones(t_list* list){
	int aux = list_size(list);
	Instruccion* instruccion = malloc(sizeof(instruccion));

	for(int i=0;i<aux;i++){
		instruccion= list_get(list,i);
		printf("El codigo de la instruccion es: %d ", instruccion->id);
		mostrar(instruccion->parametros);
	}
}

void mostrar_parametros(t_list* list){
	int aux = list_size(list);

	for(int i=0;i<aux;i++){
		int* parametro= list_get(list,i);
		printf("%d ", *parametro);
	}
	printf("\n");
}

//_-----------------------------------Funcionalidad------------------------------------------

int main(int argc, char** argv) {

	//Conecta como cliente al Kernel
	/*int sockfd = conectar_kernel();
	 saludo_inicial_kernel(sockfd);*/

	char* tamanio_proceso = argv[2];
	char* filename = argv[1];

	FILE* input_file = fopen("/home/utnso/tp-2022-1c-9-12/consola/instrucciones.txt", "r"); //harcodeado

	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;

	t_list* instrucciones = list_create();

	while (getline(&contents, &len, input_file) != -1) { //contents = contenido de la linea

		char** linea = string_split(contents, " ");
		int nombre = linea[0];
		int id = definirCodigo(nombre);

		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->id = id;
		instruccion->parametros = list_create();

		for (int i = 1;getCantidadParametros(id); i++) {
			int* parametro = malloc(sizeof(int));
			*parametro = atoi(linea[i]);
			list_add(instruccion->parametros, parametro);
		}
		list_add(instrucciones, instruccion);

	}

	mostrar_instrucciones(instrucciones);

	fclose(input_file);
	free(contents);

	//close(sockfd);
	return EXIT_SUCCESS;
}
