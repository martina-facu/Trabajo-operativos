//#include "../estados.c"
//#include <stdbool.h>
//
//void proceso_estado_new(
//    int proceso_id,
//    int proceso_tamano,
//    instruccion instrucciones[]
//   ) {
//        proceso *proceso;
//        tabla_paginas *tp;
//        pcb *proceso_pcb = pcb_create(
//            proceso_id,
//            proceso_tamano,
//            0,
//            tp,
//            0,
//            instrucciones
//        );
//        proceso = crear_proceso(proceso_pcb);
//
//        NEW[proceso_id] = proceso;
//   }
//
//
//bool hay_espacio_ready(){
//    return READY_ESPACIO_LIBRE > 0;
//}
//
//
//tabla_paginas* obtener_tabla_paginas_memoria(int proceso_id){
//    //TODO: implement
//    tabla_paginas *tp;
//    return tp;
//}
//
//void enviar_proceso_ready (int proceso_id) {
//    if(hay_espacio_ready()){
//        proceso *proceso = NEW[proceso_id];
//        READY[READY_ESPACIO_LIBRE] = proceso;
//        READY_ESPACIO_LIBRE--;
//    }
//}
//
//void long_term_planner(
//    int proceso_id,
//    int proceso_tamano,
//    instruccion instrucciones[]
//    ) {
//        proceso_estado_new(proceso_id, proceso_tamano, instrucciones);
//        tabla_paginas *tp = obtener_tabla_paginas_memoria(proceso_id);
//        enviar_proceso_ready(proceso_id);
//    }
