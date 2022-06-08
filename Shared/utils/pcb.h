#ifndef PCB
#define PCB

#include "./paquete.h"
#include "./tabla_paginas.h"
#include <commons/collections/list.h>
#include <stdlib.h>
#include "./instrucciones.h"

typedef struct {
   uint32_t pid;
   uint32_t tamano;
   uint32_t program_counter;
   uint32_t estimado_rafaga;
   Tabla_paginas *tabla_paginas;
   t_list* instrucciones;
} Pcb;

Pcb *pcb_create(
   uint32_t pid,
   uint32_t tamano,
   Tabla_paginas *tabla_paginas,
   uint32_t estimado_rafaga,
   t_list* instrucciones);

void *pcb_armar_stream(Pcb *pcb);

Pcb *pcb_deserializar(t_buffer* buffer);

void pcb_mostrar(Pcb* pcb);

uint32_t pcb_calcular_espacio(Pcb* pcb);

void* pcb_serializar(Pcb* pcb, uint32_t* tamano_mensaje);

#endif
