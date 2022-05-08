#include <stdio.h>
#include <stdlib.h>
// #include "cliente.h"
#include <sys/stat.h>
#include "./consolaConfig.h"
#include "./commons/string.h"

typedef struct {
    char* parametros[2]; 
    char* id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} instruccion;

// void exit_gracefully(int return_nr) {
//   exit(return_nr);
// }


int main(int argc, char** argv) {

	//Conecta como cliente al Kernel
	/*int sockfd = conectar_kernel();
	saludo_inicial_kernel(sockfd);*/

	char* tamanio_proceso =  argv[2];
	char* filename = argv[1];
	

	FILE* input_file = fopen(filename, "r");
    if (!input_file)
        exit(EXIT_FAILURE);

    char *contents = NULL;
    size_t len = 0;
    while (getline(&contents, &len, input_file) != -1){
		//contents = contenido de la linea
		//WRITE 4 5
		
		char**  parametros = string_split(contents, " ");

		char* id[10] = parametros[0];
		char* parametro1[10] = parametros[1];
		char* parametro2[10] = parametros[2];

        printf("%s", id);
		printf("%s", id);

    }

    fclose(input_file);
    free(contents);

	//leerInstrucciones(argv[0]);
	//close(sockfd);
	return EXIT_SUCCESS;
}


//printf("%s",argv[1]); comprobar que el path es el que se mando 
//"../intrucciones.txt"