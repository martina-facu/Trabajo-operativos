#include "pcb.h"


typedef struct 
{
    pcb *pcb;
} proceso;

proceso *crear_proceso(pcb *pcb){
    proceso *p = (proceso *) malloc(sizeof(proceso));
    p->pcb = pcb;
    return p;
};