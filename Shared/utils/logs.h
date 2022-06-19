/*
 * logs.h
 *
 *  Created on: 17 jun. 2022
 *      Author: utnso
 */

#ifndef UTILS_LOGS_H_
#define UTILS_LOGS_H_


/*
 * Listado de librerias incluidas
 */
	#include <stdio.h>
	#include <stdarg.h>
	#include <string.h>
	#include <stdlib.h>
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>
	#include <commons/string.h>

/*
 * Listado de estructuras
 */


/*
 * Listado de variables globales
*/
//	t_log* logger;



/*
 * Prototipo de funciones
*/
	t_log* initLogger(char* fileName, char* processName, t_log_level log_level);


#endif /* UTILS_LOGS_H_ */


