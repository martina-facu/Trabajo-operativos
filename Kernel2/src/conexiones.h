/*
 * conexiones.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <conexion.h>
#include "var_glob.h"

/*
 * Prototipo de funciones
*/

	void levantar_conexion_memoria(char* ipServer, char* portServer, t_log* logger);
	void levantar_conexion_cpu_interrupt(char* ipServer, char* portServer, t_log* logger);
	void levantar_conexion_cpu_dispatch(char* ipServer, char* portServer, t_log* logger);

#endif /* CONEXIONES_H_ */
