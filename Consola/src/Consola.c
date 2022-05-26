#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../../Shared/utils/instrucciones.h"
#include "../../Shared/utils/instrucciones.c"
#include "../../Shared/conexion.c"
#include "../../Shared/utils/protocolo.c"


//_-----------------------------------Funcionalidad------------------------------------------

int main() {

	//Conecta como cliente al Kernel
	/*int sockfd = conectar_kernel();
	 saludo_inicial_kernel(sockfd);*/

//	char* tamanio_proceso = argv[2];
//	char* filename = argv[1];

	FILE* input_file = fopen("instrucciones.txt", "r"); //harcodeado

	if(input_file==NULL){
		perror("error al leer el archivo");
		return -1;
	}

	char *contents = NULL;
	size_t len = 0;

	t_list* instrucciones = list_create();
	uint32_t* parametro;
	while (getline(&contents, &len, input_file) != -1) { //contents = contenido de la linea

		char** linea = string_split(contents, " ");
		char* nombre = linea[0];
		int id = definirCodigo(nombre);

		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->id = id;
		instruccion->parametros = list_create();

		for (int i = 1;linea[i]!=NULL; i++) {
			parametro = malloc(sizeof(uint32_t));
			*parametro = atoi(linea[i]);
			list_add(instruccion->parametros, parametro);
		}
		list_add(instrucciones, instruccion);

	}

	mostrar_instrucciones(instrucciones);

	fclose(input_file);
	free(contents);
	 // ---------------------------------------------------------------------------- SERIALIZACION ----------------------------------------------------------------------------------------//

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = calcular_espacio_instrucciones(instrucciones);
	void* stream = malloc(buffer->size);

	llenar_stream(instrucciones,stream);
	buffer->stream=stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion= 0;
	paquete->buffer= buffer;

	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	 // ------------------------------------------------------------------------------ CONEXION ----------------------------------------------------------------------------------------//
	t_config* config = config_create("consola.config");
	char* ip= config_get_string_value(config,"IP_KERNEL");//falta el codigo de la configuracion
	char* puerto= config_get_string_value(config,"PUERTO_KERNEL");
	int conexion= crear_conexion(ip,puerto);
	send(conexion,a_enviar,buffer->size+sizeof(uint8_t)+sizeof(uint32_t),0);
	close(conexion);
	return EXIT_SUCCESS;
}
