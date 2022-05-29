#ifndef PCB
#define PCB

#include "./paquete.h"
#include "./tabla_paginas.h"
#include <commons/collections/list.h>
#include <stdlib.h>

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

t_paquete *pcb_serializar(Pcb *pcb);

Pcb deserializar_pcb(t_paquete *paquete);

void mostrar_pcb(Pcb pcb);
#endif