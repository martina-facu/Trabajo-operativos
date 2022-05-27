/*
 ============================================================================
 Name        : Memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../../Shared/conexion.c"

int main(void) {
	t_config* config = config_create("memoria.config");

	char* puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	printf("\n puerto escucha %s",puerto);

	int socket_memoria = iniciar_servidor("127.0.0.1", puerto);
	printf("\n socket %d",socket_memoria);

	int cliente = esperar_cliente(socket_memoria);
	printf("\n cliente:  %d",cliente);

	uint8_t mensaje = 0;
	recv(cliente, &mensaje, sizeof(uint8_t), 0);
	printf("\nMensaje recibido desde el kernel: %d", mensaje);

	uint8_t handshake = 6;

	send(cliente, &handshake, sizeof(uint8_t), 0);

	close(socket_memoria);
	return EXIT_SUCCESS;
}
