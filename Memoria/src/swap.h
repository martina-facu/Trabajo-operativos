/*
 * memoria.h
 *
 *  Created on: 22 jun. 2022
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <errno.h>
#include <conexion.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h> //para funciones del swap como open
#include "memoria.h"
#include <sys/stat.h>

/*
 * Listado de estructuras
*/


/*
 * Listado de variables globales
*/

/*
 * Prototipos de funciones
*/

	//SWAP
	void iniciarSwap();
	int existe_archivo(char* path);
	void crear_archivo_swap(int pid, int tamanioProceso);
	void eliminar_archivo_swap(int pid);
	char* obtener_nombre_archivo(int pid);
	void guardar_archivo_en_swap(int pid, int tamanoProceso);
	int cerrar_archivo(char* pid, int tamanoProceso);
	void retardo_swap();


#endif /* SWAP_H_ */
