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
#include <conexion.h>
#include <instrucciones.h>

t_list* obtener_intrucciones(FILE* input_file){
	char *contents = NULL;
	size_t len = 0;

	t_list* instrucciones = list_create();
	uint32_t* parametro;
	while (getline(&contents, &len, input_file) != -1) { //contents = contenido de la linea
		char** linea = string_split(contents, " ");
		char* nombre = linea[0];
		uint8_t id = definirCodigo(nombre);

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

	fclose(input_file);
	free(contents);

	//hay que ver si hay que liberar las variables adentro de los while y de los if o que pasa con esas variables
	return instrucciones;
}

t_paquete* empaquetar_instrucciones(t_list* instrucciones){
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = calcular_espacio_instrucciones(instrucciones);
	void* stream = malloc(buffer->size);

	llenar_stream(instrucciones,stream);
	buffer->stream=stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion= 0;
	paquete->buffer= buffer;
	paquete->size = buffer->size+sizeof(uint8_t)+sizeof(uint32_t);

	return paquete;
}

void* serializar_instrucciones(t_paquete* paquete){
	t_buffer* buffer = paquete->buffer;

	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, buffer->stream, buffer->size);

	return a_enviar;
}

int main(int argc, char *argv[]) {

	/*Conecta como cliente al Kernel
	 int sockfd = conectar_kernel();
	 saludo_inicial_kernel(sockfd);*/

//	char* filename = argv[1];
//	char* tamanio_proceso = argv[2];

	FILE* input_file = fopen("instrucciones.txt", "r"); // TODO: leer los parametros de la consola

	if(input_file==NULL){
		perror("error al leer el archivo");
		return -1;
	}

	t_list* instrucciones = obtener_intrucciones(input_file);

	mostrar_instrucciones(instrucciones);

	 // ---------------------------------------------------------------------------- SERIALIZACION ----------------------------------------------------------------------------------------//
	t_paquete* paquete= empaquetar_instrucciones(instrucciones);

	void* a_enviar = serializar_instrucciones(paquete);

	 // ------------------------------------------------------------------------------ CONEXION ----------------------------------------------------------------------------------------//
	t_config* config = config_create("consola.config");
	char* ip= config_get_string_value(config,"IP_KERNEL");
	char* puerto= config_get_string_value(config,"PUERTO_KERNEL");

	int conexion= crear_conexion(ip,puerto);
	send(conexion,a_enviar,paquete->size,0);

	uint8_t respuesta = 0;
	recv(conexion,&respuesta, sizeof(uint8_t), 0);

	if(respuesta == 1){
		printf("\n termine con exito");
	}else{
		printf("\n No termine con exito");
	}

	close(conexion);
	return EXIT_SUCCESS;
}
