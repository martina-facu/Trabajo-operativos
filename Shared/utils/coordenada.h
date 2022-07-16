/*
 * coordenada.h
 *
 *  Created on: 4 jul. 2022
 *      Author: utnso
 */

#ifndef UTILS_COORDENADA_H_
#define UTILS_COORDENADA_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include "protocolo.h"

/*
 *	Listado de Estructuras
*/

	typedef struct {
		uint32_t id_tabla;
		uint32_t numero_entrada;
	} Coordenada_tabla;

	t_log* logger;
/*
 *	Prototipo de Funciones
*/

	void enviar_coordenada(Coordenada_tabla* coordenada, uint32_t* valor_buscado, int conexion, uint8_t codigo_operacion);
	void* coordenada_serializar(Coordenada_tabla* coordenada, uint32_t* tamano_mensaje, uint8_t codigo_operacion);
	void *coordenada_armar_stream(Coordenada_tabla* coordenada);
	Coordenada_tabla* coordenada_deserializar(t_buffer* buffer);
	Coordenada_tabla* recibir_coordenada(int socket_cliente);


#endif /* UTILS_COORDENADA_H_ */
