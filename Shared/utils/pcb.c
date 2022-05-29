#include "./pcb.h"

Pcb *pcb_create(
   uint32_t pid,
   uint32_t tamano,
   Tabla_paginas *tabla_paginas,
   double estimado_rafaga,
   t_list* instrucciones){
       Pcb *pcb = (Pcb *) malloc(sizeof(Pcb));
       pcb->pid = pid;
       pcb->tamano = tamano;
       pcb->program_counter = 0;
       pcb->tabla_paginas = tabla_paginas;
       pcb->estimado_rafaga = estimado_rafaga;
       pcb->instrucciones = instrucciones;
       return pcb;
   };

t_paquete *pcb_serializar(Pcb *pcb){
    t_paquete *paquete = (t_paquete *) malloc(sizeof(t_paquete));
    paquete->codigo_operacion = pcb->pid;
    paquete->buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer = paquete->buffer;
    buffer->size = sizeof(Pcb);
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, pcb, buffer->size);
    return paquete;
};

Pcb deserializar_pcb(t_paquete *paquete){
    Pcb pcb;
    memcpy(&pcb, paquete->buffer->stream, sizeof(Pcb));
    return pcb;
};

void mostrar_pcb(Pcb pcb){
    printf("PID: %d\n", pcb.pid);
    printf("TAMANO: %d\n", pcb.tamano);
    printf("PC: %d\n", pcb.program_counter);
    printf("ESTIMADO_RAFAGA: %f\n", pcb.estimado_rafaga);
    printf("INSTRUCCIONES: %d\n", pcb.instrucciones->elements_count);
};