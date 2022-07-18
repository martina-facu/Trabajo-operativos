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
#include "main.h"
#include "funciones_kernel.h"
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
	void crear_archivo_swap(int pid, int cantidadPaginas);
	void eliminar_archivo_swap(int pid);
	void swap_pagina(uint32_t pid,int numero_pagina,t_entrada_2* entrada);
	void retardo_swap();


#endif /* SWAP_H_ */
