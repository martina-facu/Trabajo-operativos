#include <stdio.h>
#include <stdlib.h>
// #include "cliente.h"
#include <sys/stat.h>
#include "./consolaConfig.h"
#include "./commons/string.h"

typedef struct {
	char* parametros[2];
	char* id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
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

int main(int argc, char** argv) {

	//Conecta como cliente al Kernel
	/*int sockfd = conectar_kernel();
	 saludo_inicial_kernel(sockfd);*/

	char* tamanio_proceso = argv[2];
	char* filename = argv[1];

	FILE* input_file = fopen(
			"/home/utnso/tp-2022-1c-9-12/consola/instrucciones.txt", "r");
	//FILE* input_file = fopen("../Debug/instrucciones.txt", "r");
	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;
	while (getline(&contents, &len, input_file) != -1) {
		//contents = contenido de la linea

		char** parametros = string_split(contents, " ");

		char* id = parametros[0];
		char* parametro1 = parametros[1];
		char* parametro2 = parametros[2];

		printf("\n%s", id);

		int codigo = -1;

		definirCodigo(id, &codigo);

		switch (codigo) {
		case 1:
			printf("\n%s", parametro1);
			break;
		case 2:
			printf("\n%s", parametro1);
			break;
		case 3:
			printf("\n%s", parametro1);
			printf("\n%s", parametro2);
			break;
		case 4:
			printf("\n%s", parametro1);
			printf("\n%s", parametro2);
			break;
		case 5:
			printf("\n%s", parametro1);
			break;
		default:
			printf("\n No hay parametros para esta operacion");
			break;
		}

		printf("\n codigo de op %d", codigo);
	}

	fclose(input_file);
	free(contents);

	//leerInstrucciones(argv[0]);
	//close(sockfd);
	return EXIT_SUCCESS;
}

//printf("%s",argv[1]); comprobar que el path es el que se mando 
//"../intrucciones.txt"
