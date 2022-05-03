#include <stdlib.h>
#include "./instruccion.h"

typedef struct {
    int numero_pagina;
    int entrada_tabla_primer_nivel;
    int entrada_tabla_segundo_nivel;
    int desplazamiento;
} tabla_paginas;

typedef struct {
    int pid;
    int tamano;
    int program_counter;
    tabla_paginas *tabla_paginas;
    double estimado_rafaga; 
    instruccion instrucciones[];
} pcb;


pcb *pcb_create(
    int pid,
    int tamano,
    int program_counter,
    tabla_paginas *tabla_paginas,
    double estimado_rafaga,
    instruccion instrucciones[]){
        pcb *process = (pcb *) malloc(sizeof(pcb));
        process->pid = pid;
        process->tamano = tamano;
        process->program_counter = program_counter;
        process->tabla_paginas = tabla_paginas;
        process->estimado_rafaga = estimado_rafaga;
        for(int i = 0; i < tamano; i++){
            process->instrucciones[i] = instrucciones[i];
        };
        return process;
    };