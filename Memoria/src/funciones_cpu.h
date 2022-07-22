/*
 * funciones_cpu.h
 *
 *  Created on: 18 jul. 2022
 *      Author: utnso
 */

#include "main.h"

#ifndef FUNCIONES_CPU_H_
#define FUNCIONES_CPU_H_

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
void clock_(t_entrada_2* entrada);
void clock_M(t_entrada_2* entrada);
void escritura();
void lectura();
void mostrar_tabla_pagina();
void chequear_puntero();
t_entrada_2* obtener_entrada();
void mostrar_entrada(t_entrada_2* entrada2);

int backup_indice_1;
int backup_entrada_tabla_1;
int backup_entrada_tabla_2;
uint32_t backup_indice_tabla_2;

#endif /* FUNCIONES_CPU_H_ */
