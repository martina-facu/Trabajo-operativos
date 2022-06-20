#include "mmu.h"

void cargar_entrada(t_list* tlb, Entrada_TLB* entrada){
	entrada->tiempo_carga = time(NULL);
	entrada->ultima_referencia = 0;
	list_add(tlb, entrada);
}

uint32_t buscar_marco(uint32_t* pagina, t_list* tlb){
	bool _coincide_pagina(void *entrada){
		return ((Entrada_TLB*)entrada)->numero_pagina == *pagina;
	}

	Entrada_TLB* entrada = list_find(tlb,_coincide_pagina);

	if(entrada != NULL){
		entrada->ultima_referencia = time(NULL); //aca se actualiza porque se pidio uso la pagina de la tlb
		return entrada->marco;
	}else{
		return -1;
	}
}

//void set_numero_pagina(Datos_calculo_direccion* datos, double direccion_logica){
//	datos->numero_pagina = floor(direccion_logica/datos->tamano_pagina);
//}
//
//void set_entrada_tabla_1er_nivel (Datos_calculo_direccion* datos){
//	datos->entrada_tabla_primer_nivel = floor(datos->numero_pagina/datos->entradas_por_tabla);
//}
//
//void set_entrada_tabla_2do_nivel (Datos_calculo_direccion* datos){
//	datos->numero_pagina = datos->numero_pagina%datos->entradas_por_tabla;
//}
//
//void set_desplazamiento (Datos_calculo_direccion* datos, double direccion_logica){
//	datos->desplazamiento = direccion_logica - (datos->numero_pagina * datos->tamano_pagina);
//}
//
//void calcular_datos_direccion(Datos_calculo_direccion* datos,double direccion_logica){
//	set_numero_pagina(datos,direccion_logica);
//	set_entrada_tabla_1er_nivel(datos);
//	set_entrada_tabla_2do_nivel(datos);
//	set_desplazamiento(datos,direccion_logica);
//}

void reemplazar_entrada(t_config* config, t_list* tlb, Entrada_TLB* entrada){
	char* algoritmo = malloc(sizeof(char) * 4);
	strcpy(algoritmo, config_get_string_value(config, "REEMPLAZO_TLB"));

	if(string_contains(algoritmo,"FIFO")){
		reemplazar_entrada_FIFO(tlb,entrada);
	}else if(string_contains(algoritmo,"LRU")){
		reemplazar_entrada_LRU(tlb,entrada);
	}else{
		printf("OCURRIO UN ERROR EN DETECTAR EL ALGORITMO DE REEMPLAZO");
	}
}

void reemplazar_entrada_FIFO(t_list* tlb, Entrada_TLB* entrada){
	bool _funcion_comparacion(void* valor1, void* valor2){
		return ((Entrada_TLB*)valor1)->tiempo_carga < ((Entrada_TLB*)valor2)->tiempo_carga;
	}

	mostrar_entradas(tlb);
	list_sort(tlb, _funcion_comparacion);
	mostrar_entradas(tlb);
	Entrada_TLB* removido = (Entrada_TLB*)list_remove(tlb, 0);
	printf("\nPagina removida: %d",removido->numero_pagina);

	cargar_entrada(tlb,entrada);
}

void reemplazar_entrada_LRU(t_list* tlb, Entrada_TLB* entrada){
	bool _funcion_comparacion(void* valor1, void* valor2){
		time_t tiempo1 = ((Entrada_TLB*)valor1)->ultima_referencia;
		time_t tiempo2 = ((Entrada_TLB*)valor2)->ultima_referencia;

		if(tiempo1 == tiempo2){
			return ((Entrada_TLB*)valor1)->tiempo_carga < ((Entrada_TLB*)valor2)->tiempo_carga;
		}else{
			return tiempo1<tiempo2;
		}
	}

	mostrar_entradas(tlb);
	list_sort(tlb, _funcion_comparacion);
	mostrar_entradas(tlb);
	Entrada_TLB* removido = (Entrada_TLB*)list_remove(tlb, 0);
	printf("\nPagina removida: %d",removido->numero_pagina);

	cargar_entrada(tlb,entrada);
}

void mostrar_entradas(t_list* list){
	int aux = list_size(list);
	Entrada_TLB* entrada = malloc(sizeof(Entrada_TLB));

	for(int i=0;i<aux;i++){
		entrada= list_get(list,i);
		printf("\nNumero de pagina: %d Marco: %d Tiempo de carga: %d Ultima referencia: %d",
				entrada->numero_pagina,
				entrada->marco,
				entrada->tiempo_carga,
				entrada->ultima_referencia);
	}

	printf("\n");
}

void mostrar_datos(Datos_calculo_direccion* datos) {
	printf("\n-----------DATOS PARA EL CALCULO DE DIRECCIONES----------------\n");
	printf("DESPLAZAMIENTO: %d\n", datos->desplazamiento);
	printf("ENTRADA PRIMER NIVEL: %d\n", datos->entrada_tabla_primer_nivel);
	printf("ENTRADA SEGUNDO NIVEL: %d\n", datos->entrada_tabla_segundo_nivel);
	printf("NUMERO PAGINA: %d\n", datos->numero_pagina);
	printf("ENTRADAS POR TABLA: %d\n", datos->entradas_por_tabla);
	printf("TAMAÑO PAGINA: %d\n", datos->tamano_pagina);
}

t_list* crear_tabla_prueba(){
	t_list* tabla = list_create();
	Entrada_TLB* entrada1 = malloc(sizeof(Entrada_TLB));
	entrada1->numero_pagina = 0;
	entrada1->marco = 0;

	Entrada_TLB* entrada2 = malloc(sizeof(Entrada_TLB));
	entrada2->numero_pagina = 1;
	entrada2->marco = 1;

	cargar_entrada(tabla,entrada1);
	sleep(1);
	cargar_entrada(tabla,entrada2);
	return tabla;
}
