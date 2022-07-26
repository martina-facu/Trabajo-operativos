/*
 * listas.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef LISTAS_H_
#define LISTAS_H_


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>

/*
 * Listado de variables globales
*/

	t_list* ready_l;
	t_list* new_l;
	t_list* newM_l;
	t_list* susp_ready_l;
	t_list* susp_readyM_l;
	t_list* susp_block_buffer_l;
	t_list* block_l;
	t_list* unblock_l;
	t_list* finalizado_l;
	t_list* susp_block_l;
	t_list* exit_l;
	t_list* interrumpidos_l;
	t_list* comunicaciones_l;
	t_list* execute_l;
	t_list* block_pend_l;


#endif /* LISTAS_H_ */
