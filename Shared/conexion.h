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


#define INICIAR_CONEXION_CPU 1
#define ACEPTAR_CONEXION_CPU 2
#define INICIAR_CONEXION_DISPATCH 21
#define ACEPTAR_CONEXION_DISPATCH 22
#define INICIAR_CONEXION_INTERRUPT 23
#define ACEPTAR_CONEXION_INTERRUPT 24
#define SOLICITAR_INTERRUPCION 25
#define INICIAR_CONEXION_KERNEL 41
#define ACEPTAR_CONEXION_KERNEL 42
#define INICIAR_CONEXION_CONSOLA 61
#define ACEPTAR_CONEXION_CONSOLA 62
#define PROCESO_FINALIZADO 63



int crear_conexion(char *ip, char* puerto);
int iniciar_servidor(char *ip, char* puerto_escucha);
int esperar_cliente(int socket_servidor);


#endif
