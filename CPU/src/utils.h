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

Pcb* obtener_pcb(int socket_serv, int cliente);

int levantar_conexion_memoria(t_config* config);

int levantar_canal_dispatch(t_config* config, int* socket_dispatch);

int levantar_puerto_interrupt(t_config* config, int* socket_interrupt);

int execute(Instruccion* instruccion,t_config* config);

#endif
