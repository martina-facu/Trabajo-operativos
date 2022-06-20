#include "mmu.h"

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

void cargar_entrada(t_config* config, t_list* tlb, Entrada_TLB* entrada){
	uint32_t* entradas_maximas = malloc(sizeof(uint32_t));
	entrada->tiempo_carga = time(NULL);
	entrada->ultima_referencia = 0;

//	*entradas_maximas = config_get_int_value(config, "ENTRADAS_TLB");
	*entradas_maximas = 4;

	int tamano_lista = list_size(tlb);
	if(tamano_lista < *entradas_maximas){
		list_add(tlb, entrada);
	}else{
		reemplazar_entrada(config,tlb,entrada);
	}
}

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

	list_add(tlb, entrada);
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

	list_add(tlb, entrada);
}

void set_numero_pagina(Datos_calculo_direccion* datos, double direccion_logica){
	datos->numero_pagina = direccion_logica/datos->tamano_pagina;
}

void set_entrada_tabla_1er_nivel (Datos_calculo_direccion* datos){
	datos->entrada_tabla_primer_nivel = datos->numero_pagina/datos->entradas_por_tabla;
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



double traducir_direccion(double direccion_logica, uint32_t id_tabla_paginas1, int conexion_memoria,
		Entrada_TLB* entrada_resultante){// el id de la tabla es el que viene del kernel

	Datos_calculo_direccion* datos = malloc(sizeof(Datos_calculo_direccion));
	calcular_datos_direccion(datos,direccion_logica);
	datos->id_tabla_paginas1 = id_tabla_paginas1;

	mostrar_datos(datos);
	entrada_resultante->marco = get_marco(datos,conexion_memoria);

	return entrada_resultante->marco + datos->desplazamiento;
}

uint32_t get_marco(Datos_calculo_direccion* datos, int conexion_memoria){
	uint32_t* id_tabla_paginas2 = malloc(sizeof(uint32_t));

	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));
	coordenada->id_tabla = datos->id_tabla_paginas1;
	coordenada->numero_entrada = datos->entrada_tabla_primer_nivel;

	enviar_coordenada(coordenada, id_tabla_paginas2, conexion_memoria);

	coordenada->id_tabla = *id_tabla_paginas2;
	coordenada->numero_entrada = datos->entrada_tabla_segundo_nivel;

	uint32_t* marco = malloc(sizeof(uint32_t));
	enviar_coordenada(coordenada, marco, conexion_memoria);

	return *marco;
}

void enviar_coordenada(Coordenada_tabla* coordenada, uint32_t* valor_buscado, int conexion){
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
	void* a_enviar = pcb_serializar(coordenada,tamano_mensaje,1);
	send(conexion, a_enviar, *tamano_mensaje, 0);

	recv(conexion, valor_buscado, sizeof(uint32_t), 0);
}

void *coordenada_armar_stream(Coordenada_tabla* coordenada) {
	int tamano = sizeof(Coordenada_tabla);
	void* stream = malloc(tamano);

	int desplazamiento = 0;

	memcpy(stream, &coordenada->id_tabla, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &coordenada->numero_entrada, sizeof(uint32_t));

	return stream;
}
;

void* coordenada_serializar(Coordenada_tabla* coordenada, uint32_t* tamano_mensaje, uint32_t codigo_operacion) {
	void* stream_coordenada = coordenada_armar_stream(coordenada);
	uint32_t tamano = sizeof(uint32_t) * 2;

	t_buffer* buffer = armar_buffer(tamano, stream_coordenada);
	t_paquete* paquete = empaquetar_buffer(buffer, codigo_operacion);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

	*tamano_mensaje = paquete->size;
	return a_enviar;
}

Coordenada_tabla* coordenada_deserializar(t_buffer* buffer) {
	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));
	void* stream = buffer->stream;

	memcpy(&(coordenada->id_tabla), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(coordenada->numero_entrada), stream, sizeof(uint32_t));

	return coordenada;
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

t_list* crear_tabla_prueba(t_config* config){
	t_list* tabla = list_create();
	Entrada_TLB* entrada1 = malloc(sizeof(Entrada_TLB));
	entrada1->numero_pagina = 0;
	entrada1->marco = 0;

	Entrada_TLB* entrada2 = malloc(sizeof(Entrada_TLB));
	entrada2->numero_pagina = 1;
	entrada2->marco = 1;

	Entrada_TLB* entrada3 = malloc(sizeof(Entrada_TLB));
	entrada3->numero_pagina = 2;
	entrada3->marco = 2;

	Entrada_TLB* entrada4 = malloc(sizeof(Entrada_TLB));
	entrada4->numero_pagina = 3;
	entrada4->marco = 3;

	cargar_entrada(config,tabla,entrada1);
	sleep(1);
	cargar_entrada(config,tabla,entrada2);
	sleep(1);
	cargar_entrada(config,tabla,entrada3);
	cargar_entrada(config,tabla,entrada4);

	return tabla;
}
