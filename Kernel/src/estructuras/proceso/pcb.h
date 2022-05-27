//#include <stdlib.h>
//#include "../../../../Shared/utils/instrucciones.h"
//
//typedef struct {
//    int numero_pagina;
//    int entrada_tabla_primer_nivel;
//    int entrada_tabla_segundo_nivel;
//    int desplazamiento;
//} tabla_paginas;
//
//typedef struct {
//    int pid;
//    int tamano;
//    int program_counter;
//    tabla_paginas *tabla_paginas;
//    double estimado_rafaga;
//    instruccion instrucciones[];
//} pcb;
//
//
//pcb *pcb_create(
//    int pid,
//    int tamano,
//    int program_counter,
//    tabla_paginas *tabla_paginas,
//    double estimado_rafaga,
//    instruccion instrucciones[]){
//        pcb *proceso = (pcb *) malloc(sizeof(pcb));
//        proceso->pid = pid;
//        proceso->tamano = tamano;
//        proceso->program_counter = program_counter;
//        proceso->tabla_paginas = tabla_paginas;
//        proceso->estimado_rafaga = estimado_rafaga;
//        for(int i = 0; i < tamano; i++){
//            proceso->instrucciones[i] = instrucciones[i];
//        };
//        return proceso;
//    };
