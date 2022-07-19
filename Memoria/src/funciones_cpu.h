/*
 * funciones_cpu.h
 *
 *  Created on: 18 jul. 2022
 *      Author: utnso
 */

#include "main.h"

#ifndef FUNCIONES_CPU_H_
#define FUNCIONES_CPU_H_

uint32_t entrada_backup_tabla_1;
uint32_t pid_;
bool memoria_esta_llena();
void entrada1();
void entrada2();
void* funciones_cpu();
void page_fault(t_entrada_2* entrada,uint32_t entrada_backup_tabla_1, uint32_t entrada_tabla_2);
int buscar_frame_libre();
void ejecutar_algoritmo( t_entrada_2* entrada);
t_proceso* proceso_;
bool ordenar(void* entrada1, void* entrada2);
t_memory_pag* clock_();

#endif /* FUNCIONES_CPU_H_ */
