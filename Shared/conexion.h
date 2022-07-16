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
#include <commons/log.h>


#define INICIAR_CONEXION_CPU 1
#define ACEPTAR_CONEXION_CPU 2
#define SOLICITAR_ENTRADA_Y_TAMANO 3
#define ENTRADAS_Y_TAMANIO_DE_PAGINA 4
#define SOLICITAR_VALOR_ENTRADA1 5
#define ID_TABLA_SEGUNDO_NIVEL 6
#define SOLICITAR_VALOR_ENTRADA2 7
#define MARCO_DE_LA_ENTRADA 8
#define SOLICITAR_LECTURA 9
#define RESULTADO_LECTURA 10
#define SOLICITAR_ESCRITURA 11
#define RESULTADO_ESCRITURA 12
#define INICIAR_CONEXION_DISPATCH 21
#define ACEPTAR_CONEXION_DISPATCH 22
#define INICIAR_CONEXION_INTERRUPT 23
#define ACEPTAR_CONEXION_INTERRUPT 24
#define SOLICITAR_INTERRUPCION 25
#define INICIAR_CONEXION_KERNEL 41
#define ACEPTAR_CONEXION_KERNEL 42
#define INICIALIZAR_PROCESO 43
#define ACCEDER_TABLA_DE_PAGINAS 200
#define ACCEDER_ESPACIO_DE_USUARIO 300
#define SUSPENDER_PROCESO 45
#define FINALIZAR_PROCESO 47
#define INICIAR_CONEXION_CONSOLA 61
#define ACEPTAR_CONEXION_CONSOLA 62
#define PROCESO_FINALIZADO 63

int crear_conexion(char *ip, char* puerto, t_log* logger);
int iniciar_servidor(char *ip, char* puerto_escucha, t_log* logger);
int esperar_cliente(int socket_servidor, t_log* logger);

#endif
