/*
 * algoritmos.h
 *
 *  Created on: 5 jul. 2022
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include "memoria.h"
#include "mensajes.h"


	//ALGORITMOS DE REEMPLAZO
	int sacar_con_clock_modificado(int pid);
	int sacar_con_clock(int pid);

	int buscar_ultima_posicion_sacada(int pid);
	void cambiar_ultima_posicion(int pid, int posicion);
	t_proceso* buscar_tabla (int pid);
	void aplicar_algoritmo_reemplazo(t_proceso* proceso);
	int cantidad_de_paginas_del_proceso_en_memoria(int pid);


#endif /* ALGORITMOS_H_ */
