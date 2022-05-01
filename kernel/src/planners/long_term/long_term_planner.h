#include "../states.h"
#include <stdbool.h>

void new_process( 
    int process_id,
    int process_size, 
    char *instructions[]   
   ) {
        process *process;
        pages_table *pt;
        pcb *process_pcb = pcb_create(
            process_id,
            process_size,
            0,
            pt,
            0,
            instructions
        );
        process = process_create(process_pcb);
        
        NEW[process_id] = process;
   }


bool has_ready_state_space(){
    return READY_FREE_SPACE > 0;
}


pages_table* get_pages_table_from_memory(int process_id){
    //TODO: implement
    pages_table *pt;
    return pt;
}

void change_to_ready (int process_id) {
    if(has_ready_state_space()){
        process *process = NEW[process_id];
        READY[READY_FREE_SPACE] = process;
        READY_FREE_SPACE--;
    }
}

void long_term_planner(
    int process_id,
    int process_size, 
    char *instructions[]   
    ) {
        new_process(process_id, process_size, instructions);
        pages_table *pt = get_pages_table_from_memory(process_id);
        change_to_ready(process_id);   
    }
