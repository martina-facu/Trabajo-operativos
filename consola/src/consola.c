#include <stdio.h>
#include <stdlib.h>
// #include "cliente.h"
#include <sys/stat.h>
#include "./consolaConfig.h"
#include "./commons/string.h"
#include "./commons/collections/queue.h"

typedef struct {
	char* parametros[2];
	int id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} Instruccion;

// void exit_gracefully(int return_nr) {
//   exit(return_nr);
// }

void definirCodigo(char* id, int* codigo) {
	if (string_contains(id, "NO_OP")) {
		*codigo = 1;
	}
	if (string_contains(id, "I/O")) {
		*codigo = 2;
	}
	if (string_contains(id, "WRITE")) {
		*codigo = 3;
	}
	if (string_contains(id, "COPY")) {
		*codigo = 4;
	}
	if (string_contains(id, "READ")) {
		*codigo = 5;
	}
	if (string_contains(id, "EXIT")) {
		*codigo = 6;
	}
}

int definirCantidadParametros(codigo){
	switch (codigo) {
		case 1:
			return 1;
		break;
		case 2:
			return 1;
		break;
		case 3:
			return 2;
		break;
		case 4:
			return 2;
		break;
		case 5:
			return 1;
		break;
		case 5:
			return 0;
		break;
		default:
		printf("\n No hay parametros para esta operacion");
		break;
	}
}


int main(int argc, char** argv) {

	//Conecta como cliente al Kernel
	/*int sockfd = conectar_kernel();
	 saludo_inicial_kernel(sockfd);*/

	char* tamanio_proceso = argv[2];
	char* filename = argv[1];

	FILE* input_file = fopen("/home/utnso/tp-2022-1c-9-12/consola/instrucciones.txt", "r");//harcodeado

	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;

	t_queue * instrucciones = queue_create();

	while (getline(&contents, &len, input_file) != -1) {//contents = contenido de la linea


		char** parametros = string_split(contents, " ");

		char* id = parametros[0];
		char* parametro1 = atoi(parametros[1]);
		char* parametro2 = atoi(parametros[2]); //dudoso para saber si te va a venir null o no

		printf("\n%s", id);

		int codigo = -1;

		definirCodigo(id, &codigo);


		printf("\n codigo de op %d", codigo);
	}

	fclose(input_file);
	free(contents);

	//close(sockfd);
	return EXIT_SUCCESS;
}
