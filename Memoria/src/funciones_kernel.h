/*
 * funciones_kernel.h
 *
 *  Created on: 17 jul. 2022
 *      Author: utnso
 */
#include "main.h"

#ifndef FUNCIONES_KERNEL_H_
#define FUNCIONES_KERNEL_H_


typedef struct{
	uint32_t pid;
	int* entradas;
}t_tabla_1;

typedef struct{
	t_list* entradas;
}t_tabla_2;

typedef struct{
	int frame;
	uint32_t bUso;
	uint32_t bMod;
	uint32_t bPres;
}t_entrada_2;

typedef struct{
	uint32_t n_tabla_2;
	uint32_t n_entrada_2;
	t_entrada_2 *entrada;
}t_memory_pag;

typedef struct{
	uint32_t pid;
	uint32_t tam_proceso;
	uint32_t entrada_1;
	t_list* pagMem;
	int puntero;
	int contador;
}t_proceso;

typedef struct{
	uint32_t pid;
	t_list* memorias_a_swappear;
}t_swap;

int division_entera(double numerador,  double denominador);
void* funciones_kernel();
void inicializar_proceso();
bool paginas_modificadas(void* entrada_);
void suspender_proceso();
void mostrar_paginas(t_list* paginas);
t_proceso* crear_proceso(uint32_t pid, uint32_t tam_proceso);
void inicializar_tabla_1(t_tabla_1* tabla,uint32_t pid);
void asignar_tabla_2(t_tabla_1* tabla,uint32_t cant_entradas_1);
void inicializar_tabla_2(t_list* entradas_tabla2);
void finalizar_proceso();
void mostrar_bitarray();


t_list* tabla_1_l;
t_list* tabla_2_l;
t_list* procesos;
t_list* pedidos_swap_l;


#endif /* FUNCIONES_KERNEL_H_ */
