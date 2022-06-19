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

void set_numero_pagina(Datos_calculo_direccion* datos, double direccion_logica){
	datos->numero_pagina = floor(direccion_logica/datos->tamano_pagina);
}

void set_entrada_tabla_1er_nivel (Datos_calculo_direccion* datos){
	datos->entrada_tabla_primer_nivel = floor(datos->numero_pagina/datos->entradas_por_tabla);
}

void set_entrada_tabla_2do_nivel (Datos_calculo_direccion* datos){
	datos->numero_pagina = datos->numero_pagina%datos->entradas_por_tabla;
}

void set_desplazamiento (Datos_calculo_direccion* datos, double direccion_logica){
	datos->desplazamiento = direccion_logica - (datos->numero_pagina * datos->tamano_pagina);
}

void calcular_datos_direccion(Datos_calculo_direccion* datos,double direccion_logica){
	set_numero_pagina(datos,direccion_logica);
	set_entrada_tabla_1er_nivel(datos);
	set_entrada_tabla_2do_nivel(datos);
	set_desplazamiento(datos,direccion_logica);
}

void reemplazar_entrada(t_config* config, t_list* tlb){
	char* algoritmo = malloc(sizeof(char) * 4);
	strcpy(algoritmo, config_get_string_value(config, "REEMPLAZO_TLB"));

	if(algoritmo == "FIFO"){
		reemplazar_entrada_FIFO(tlb);
	}else if(algoritmo == "LRU"){
		reemplazar_entrada_LRU(tlb);
	}else{
		printf("OCURRIO UN ERROR EN DETECTAR EL ALGORITMO DE REEMPLAZO");
	}
}

void reemplazar_entrada_FIFO(t_list* tlb){

}

void reemplazar_entrada_LRU(t_list* tlb){

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
	cargar_entrada(tabla,entrada2);
	return tabla;
}
