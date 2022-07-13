/*
 * memoria.h
 *
 *  Created on: 22 jun. 2022
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#include "memoria.h"
#include "swap.h"
#include <coordenada.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <errno.h>
#include <conexion.h>
#include <logs.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h> //para funciones del swap como open
#include <pcb.h>

/*
 * Listado de estructuras
*/

/*
 * Listado de variables globales
*/

/*
 * Prototipos de funciones
*/


	//MENSAJES
	void inicializar_proceso(int socket_cliente);
	t_list* buscar_frames_sin_ocupar();
	void suspender_proceso(int socket_cliente);

	void finalizar_proceso(int socket_cliente);
	void iniciar_eliminacion_proceso(int pid);
	void acceder_tabla_de_paginas(int socket_cliente);
	void acceder_espacio_de_usuario(int socket_cliente);

	void entradas_y_tamanio_de_pagina(int socket_cliente);
	void liberar_memoria_suspencion(t_proceso* proceso);

	void devolver_numero_tabla_segundo_nivel(int socket_cliente);
	void devolver_numero_marco_asociado(int socket_cliente);
	void devolver_lectura(int socket_cliente);
	void devolver_escritura(int socket_cliente);

	
	void iniciar_proceso(pcb_t* pcb);
//TODO VER SI BORRAR ESTA
	uint32_t obtener_numero_frame(int nroIndiceTablaSegundoNivel);

	uint32_t cargar_pagina_en_algun_marco(t_tabla_paginas_segundo_nivel* nroTablaSegundoNivel);

#endif /* MENSAJES_H_ */
