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
	uint32_t pid;
	uint32_t tam_proceso;
	uint32_t entrada_1;
	t_list* pagMem;
	int puntero;
	int contador;
}t_proceso;

typedef struct{
	int frame;
	uint32_t bUso;
	uint32_t bMod;
	uint32_t bPres;
}t_entrada_2;

int division_entera(double numerador,  double denominador);
void* funciones_kernel();
void inicializar_proceso();
t_proceso* crear_proceso(uint32_t pid, uint32_t tam_proceso);
void inicializar_tabla_1(t_tabla_1* tabla,uint32_t pid);
void asignar_tabla_2(t_tabla_1* tabla,uint32_t cant_entradas_1);
void inicializar_tabla_2(t_list* entradas_tabla2);


t_list* tabla_1_l;
t_list* tabla_2_l;
t_list* procesos;


#endif /* FUNCIONES_KERNEL_H_ */
