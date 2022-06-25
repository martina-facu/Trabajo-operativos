#ifndef CPU
#define CPU

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <conexion.h>
#include <pcb.h>
#include <paquete.h>
#include <mmu.h>

int conexion_memoria;
uint32_t* cantidad_entradas;
uint32_t* tamano_pagina;
bool* hubo_interrupcion;
t_config* config_cpu;

void inicializar_utils(t_config* config, int conexion_m, bool* interrupcion);

Pcb* obtener_pcb(int socket_serv, int cliente);

int levantar_conexion_memoria(t_config* config);

int levantar_canal_dispatch(t_config* config, int* socket_dispatch);

int levantar_puerto_interrupt(t_config* config, int* socket_interrupt);

void ejecutar_ciclo_instrucciones(Pcb* pcb,t_config* config, bool* devolver_pcb);

bool execute(Instruccion* instruccion,t_config* config, Pcb* pcb);

uint32_t* leer(int direccion_logica, Datos_calculo_direccion* datos);

uint32_t* escribir(int direccion_logica, uint32_t* valor_a_escribir, Datos_calculo_direccion* datos);

#endif
