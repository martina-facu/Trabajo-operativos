#include "pcb.h"


typedef struct 
{
    pcb *pcb;
} process;

process *process_create(pcb *pcb){
    process *p = (process *) malloc(sizeof(process));
    p->pcb = pcb;
    return p;
};