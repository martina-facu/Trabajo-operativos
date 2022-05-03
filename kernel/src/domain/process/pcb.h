#include <stdlib.h>
#include "../instruccions/instruccion.h"

typedef struct {
    int page_number;
    int table_input_first_level;
    int table_input_second_level;
    int displacement;
} pages_table;

typedef struct {
    int pid;
    int size;
    int program_counter;
    pages_table *pages_table;
    double brust_stimate; 
    instruccion instructions[]; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} pcb;


pcb *pcb_create(
    int pid,
    int size,
    int program_counter,
    pages_table *pages_table,
    double brust_stimate,
    instruccion instructions[]){
        pcb *process = (pcb *) malloc(sizeof(pcb));
        process->pid = pid;
        process->size = size;
        process->program_counter = program_counter;
        process->pages_table = pages_table;
        process->brust_stimate = brust_stimate;
        for(int i = 0; i < size; i++){
            process->instructions[i] = instructions[i];
        };
        return process;
    };