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
       pcb->estimado_rafaga = estimado_rafaga;
       pcb->tabla_paginas = tabla_paginas;
       pcb->instrucciones = instrucciones;
       return pcb;
   };

uint32_t pcb_calcular_espacio(Pcb* pcb){
	uint32_t tamano_instrucciones = calcular_espacio_instrucciones(pcb->instrucciones);
	uint32_t tamano_tabla_paginas = sizeof(Tabla_paginas);
    return tamano_tabla_paginas+
            tamano_instrucciones +
            sizeof(uint32_t) * 3; // PDI, TAMANO, ESTIMACION
};


void *pcb_armar_stream(Pcb *pcb){
    int tamano_pcb = pcb_calcular_espacio(pcb);
    void* stream = malloc(tamano_pcb);

    int desplazamiento=0;

    memcpy(stream,&pcb->pid,sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    memcpy(stream+desplazamiento,&pcb->tamano,sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    memcpy(stream+desplazamiento,&pcb->tabla_paginas->numero_pagina,sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    memcpy(stream+desplazamiento,&pcb->tabla_paginas->entrada_tabla_primer_nivel,sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    memcpy(stream+desplazamiento,&pcb->tabla_paginas->entrada_tabla_segundo_nivel,sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    memcpy(stream+desplazamiento,&pcb->tabla_paginas->desplazamiento,sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    memcpy(stream+desplazamiento,&pcb->estimado_rafaga,sizeof(double));
    desplazamiento+=sizeof(double);

    memcpy(stream+desplazamiento,armar_stream_instruccion(pcb->instrucciones),calcular_espacio_instrucciones(pcb->instrucciones));

    return stream;
};

Pcb* pcb_deserializar(t_buffer* buffer){
    Pcb* pcb = malloc(sizeof(Pcb));
    Tabla_paginas* pagina = malloc(sizeof(Tabla_paginas));
    void* stream = buffer->stream;
    memcpy(&(pcb->pid),stream,sizeof(uint32_t));
    stream+=sizeof(uint32_t);
    memcpy(&(pcb->tamano),stream,sizeof(uint32_t));
    stream+=sizeof(uint32_t);
    memcpy(&(pagina->numero_pagina),stream,sizeof(uint32_t));
    stream+=sizeof(uint32_t);
    memcpy(&(pagina->entrada_tabla_primer_nivel),stream,sizeof(uint32_t));
    stream+=sizeof(uint32_t);
    memcpy(&(pagina->entrada_tabla_segundo_nivel),stream,sizeof(uint32_t));
    stream+=sizeof(uint32_t);
    memcpy(&(pagina->desplazamiento),stream,sizeof(uint32_t));
    stream+=sizeof(uint32_t);
    memcpy(&(pcb->estimado_rafaga),stream,sizeof(double));
    stream+=sizeof(uint32_t);
    buffer->stream = stream;
    pcb->tabla_paginas= pagina;
    t_list* instrucciones= list_create();
    deserializar_instrucciones(buffer,instrucciones);
    pcb->instrucciones= instrucciones;
    return pcb;
};

void pcb_mostrar(Pcb* pcb){
	printf("\n\nINFORMACION PCB:\n");
    printf("PID: %d\n", pcb->pid);
    printf("TAMANO: %d\n", pcb->tamano);
    printf("PC: %d\n", pcb->program_counter);
    printf("ESTIMADO_RAFAGA: %f\n", pcb->estimado_rafaga);
    tabla_paginas_mostrar(pcb->tabla_paginas);
    mostrar_instrucciones(pcb->instrucciones);
};
