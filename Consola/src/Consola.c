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
#include <paquete.h>

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


int main(int argc, char *argv[]) {

	char* filename = argv[1];

	uint32_t* tamano_proceso = malloc(sizeof(uint32_t));
	*tamano_proceso = atoi(argv[2]);

//	FILE* input_file = fopen("instrucciones.txt", "r");
	FILE* input_file = fopen(filename, "r");

	if(input_file==NULL){
		perror("error al leer el archivo");
		return -1;
	}

	t_list* instrucciones = obtener_intrucciones(input_file);

	mostrar_instrucciones(instrucciones);


	 // ---------------------------------------------------------------------------- SERIALIZACION ----------------------------------------------------------------------------------------//
	t_buffer* buffer= intrucciones_armar_buffer(instrucciones);
	t_paquete* paquete= empaquetar_buffer(buffer);

	int offset = 0;
	void* a_enviar = serializar_paquete(paquete);
	offset += paquete->size;

	memcpy(a_enviar + offset,tamano_proceso,sizeof(uint32_t));


	 // ------------------------------------------------------------------------------ CONEXION ----------------------------------------------------------------------------------------//
	t_config* config = config_create("consola.config");
	char* ip= config_get_string_value(config,"IP_KERNEL");
	char* puerto= config_get_string_value(config,"PUERTO_KERNEL");

	int conexion= crear_conexion(ip,puerto);
	send(conexion,a_enviar,paquete->size + sizeof(uint32_t),0);

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
