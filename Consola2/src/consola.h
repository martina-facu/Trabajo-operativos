/*
 * consola.h
 *
 *  Created on: 26 jun. 2022
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

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
#include <protocolo.h>
#include <logs.h>

/*
 * Estructuras
*/

/*
 * Listado de variables globales
*/

/*
 * Prototipo de funciones
*/

void* serializar_mensaje(t_list* instrucciones, uint32_t* tamano_proceso,uint32_t* tamano_mensaje);
t_list* obtener_intrucciones(FILE* input_file);
int levantarConexionKernel(char* ip, char* puerto, t_log* logger);

#endif /* CONSOLA_H_ */
