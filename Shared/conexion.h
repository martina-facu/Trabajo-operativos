#ifndef CONEXION
#define CONEXION

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

int crear_conexion(char *ip, char* puerto);
int iniciar_servidor(char *ip, char* puerto_escucha);
int esperar_cliente(int socket_servidor);


#endif
