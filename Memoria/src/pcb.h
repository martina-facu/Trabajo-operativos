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
   Tabla_paginas *tabla_paginas;
   double estimado_rafaga;
   t_list* instrucciones;
} Pcb;

Pcb *pcb_create(
   uint32_t pid,
   uint32_t tamano,
   Tabla_paginas *tabla_paginas,
   double estimado_rafaga,
   t_list* instrucciones);

void *pcb_serializar(Pcb *pcb, void* stream);

Pcb *pcb_deserializar(t_buffer* buffer);

void pcb_mostrar(Pcb* pcb);

t_buffer* crear_buffer(void* stream, int tamano);

t_paquete* pcb_empaquetar(t_buffer* buffer);

int pcb_calcular_espacio(Pcb* pcb);
#endif
