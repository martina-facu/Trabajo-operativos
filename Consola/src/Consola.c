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

t_list* obtener_intrucciones(FILE* input_file) {
	char *contents = NULL;
	size_t len = 0;

	t_list* instrucciones = list_create();
	uint32_t* parametro;
	uint32_t* cant = malloc(sizeof(uint32_t));

	while (getline(&contents, &len, input_file) != -1) {
		char** linea = string_split(contents, " ");
		char* nombre = linea[0];
		uint8_t id = definirCodigo(nombre);

		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->id = id;
		instruccion->parametros = list_create();

		if(id == 1){
			*cant = atoi(linea[1]);

			for (int i = 1; i<=*cant; i++) {
				list_add(instrucciones, instruccion);
			}
		}else{
			for (int i = 1; linea[i] != NULL; i++) {
				parametro = malloc(sizeof(uint32_t));
				*parametro = atoi(linea[i]);
				list_add(instruccion->parametros, parametro);
			}
			list_add(instrucciones, instruccion);
		}
	}

	fclose(input_file);
	free(contents);
	free(cant);

	return instrucciones;
}

void* serializar_mensaje(t_list* instrucciones, uint32_t* tamano_proceso,uint32_t* tamano_mensaje) {
	void* stream_instrucciones = armar_stream_instruccion(instrucciones);
	int tamano_instrucciones = calcular_espacio_instrucciones(instrucciones);

	t_buffer* buffer = armar_buffer(tamano_instrucciones, stream_instrucciones);
	t_paquete* paquete = empaquetar_buffer(buffer,0);

	void* a_enviar = malloc(paquete->size + sizeof(uint32_t));
	a_enviar = serializar_paquete(paquete, a_enviar);

	memcpy(a_enviar + paquete->size, tamano_proceso, sizeof(uint32_t));

	*tamano_mensaje = paquete->size + sizeof(uint32_t);

	return a_enviar;
}

int main(int argc, char *argv[]) {

	char* filename = argv[1];
	t_log* logger;

	logger = initLogger("consola.log", "CONSOLA", LOG_LEVEL_INFO);

	uint32_t* tamano_proceso = malloc(sizeof(uint32_t));
	*tamano_proceso = atoi(argv[2]);

	FILE* input_file = fopen(filename, "r");

	if (input_file == NULL) {
		perror("error al leer el archivo");
		return -1;
	}

	t_list* instrucciones = obtener_intrucciones(input_file);

	mostrar_instrucciones(instrucciones, logger);

	// ---------------------------------------------------------------------------- SERIALIZACION ----------------------------------------------------------------------------------------//

	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
	void* a_enviar = serializar_mensaje(instrucciones,tamano_proceso,tamano_mensaje);

	// ---------------------------------------------------------------------------- CONEXION ----------------------------------------------------------------------------------------//
	t_config* config = config_create("consola.config");
	char* ip = config_get_string_value(config, "IP_KERNEL");
	log_info(logger, "La IP de conexion al Kernel es: %s", ip);
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");
	log_info(logger, "El puerto de conexion al Kernel es: %s", puerto);

	int conexion = crear_conexion(ip, puerto, logger);
	if (conexion < 0)
		exit(EXIT_FAILURE);
	log_info(logger, "Me conecte al KERNEL en el descriptor: %d", conexion);
	send(conexion, a_enviar,*tamano_mensaje, 0);
	log_info(logger, "Se envio el mensaje %s al KERNEL", a_enviar);


	uint8_t respuesta = 0;
	recv(conexion, &respuesta, sizeof(uint8_t), 0);
	log_info(logger, "Se recibio la respuesta %d del KERNEL", respuesta);


	if (respuesta == 1) {
		printf("\n termine con exito");
	} else {
		printf("\n No termine con exito");
	}

	close(conexion);
	return EXIT_SUCCESS;
}
