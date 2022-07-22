/*
 * mmu.c
 *
 *  Created on: 4 jul. 2022
 *      Author: utnso
 */


#include "mmu.h"

void inicializar_mmu(t_config* config, t_list* tlb, t_log* logger_cpu){
	config_cpu = config;
	tlb_proceso = tlb;
	logger = logger_cpu;
}

uint32_t buscar_marco(uint32_t pagina){
	bool _coincide_pagina(void *entrada){
		return ((Entrada_TLB*)entrada)->numero_pagina == pagina;
	}

	log_trace(logger, "CPU-MMU || Obtengo la entrada ");

	Entrada_TLB* entrada = list_find(tlb_proceso,_coincide_pagina);

	if(entrada != NULL){
		log_trace(logger, "CPU-MMU || Encontre el marco %d de la pagina %d", entrada->marco, entrada->numero_pagina);
		log_trace(logger, "CPU-MMU || Actualizo la ultima referencia %d", entrada->ultima_referencia);
		entrada->ultima_referencia = time(NULL); //aca se actualiza porque se uso la pagina de la tlb
		log_info(logger, "CPU-MMU || Referencia actualizada: %d", entrada->ultima_referencia);
		return entrada->marco;
	}else{
		return -1;
	}
}

void cargar_entrada(Entrada_TLB* entrada)
{
	entrada->tiempo_carga = time(NULL);
	entrada->ultima_referencia = 0;

	uint32_t entradas_maximas = config_get_int_value(config_cpu,"ENTRADAS_TLB");

	int tamano_lista = list_size(tlb_proceso);

	if(tamano_lista < entradas_maximas){
		log_trace(logger, "CPU-MMU || Agrego una entrada a la TLB");
		list_add(tlb_proceso, entrada);
		mostrar_entradas(tlb_proceso);
	}else{
		log_trace(logger, "CPU-MMU || Voy a reemplazar una entrada");
		reemplazar_entrada(entrada);
	}

	log_info(logger, "CPU-MMU Se cargo una entrada a la tlb");
}

void reemplazar_entrada(Entrada_TLB* entrada){
	char* algoritmo = malloc(sizeof(char) * 4);
	strcpy(algoritmo, config_get_string_value(config_cpu, "REEMPLAZO_TLB"));

	if(string_contains(algoritmo,"FIFO")){
		reemplazar_entrada_FIFO(entrada);
	}else if(string_contains(algoritmo,"LRU")){
		reemplazar_entrada_LRU(entrada);
	}else{
		log_error(logger, "CPU-MMU || OCURRIO UN ERROR EN DETECTAR EL ALGORITMO DE REEMPLAZO");
	}
	free(algoritmo);
}

void reemplazar_entrada_FIFO(Entrada_TLB* entrada){
	bool _funcion_comparacion(void* valor1, void* valor2){
		return ((Entrada_TLB*)valor1)->tiempo_carga < ((Entrada_TLB*)valor2)->tiempo_carga;
	}
	log_info(logger, "CPU-MMU || Algoritmo de reemplazo: FIFO");
	mostrar_entradas(tlb_proceso);
	log_trace(logger, "CPU-MMU || REORDENO TLB");
	list_sort(tlb_proceso, _funcion_comparacion);
	mostrar_entradas(tlb_proceso);
	Entrada_TLB* removido = (Entrada_TLB*)list_remove(tlb_proceso, 0);

	log_info(logger, "CPU-MMU Pagina removida: %d",removido->numero_pagina);

	list_add(tlb_proceso, entrada);
}

void reemplazar_entrada_LRU(Entrada_TLB* entrada){
	bool _funcion_comparacion(void* valor1, void* valor2){
		time_t tiempo1 = ((Entrada_TLB*)valor1)->ultima_referencia;
		time_t tiempo2 = ((Entrada_TLB*)valor2)->ultima_referencia;
		log_info(logger, "CPU-MMU || Algoritmo de reemplazo: LRU");
		if(tiempo1 == tiempo2){
			log_trace(logger, "CPU-MMU || Tiempo de carga valor 1 es menor al 2");
			return ((Entrada_TLB*)valor1)->tiempo_carga < ((Entrada_TLB*)valor2)->tiempo_carga;
		}else{
			return tiempo1<tiempo2;
		}
	}
	log_trace(logger, "CPU-MMU || Se muestra la TLB");
	mostrar_entradas(tlb_proceso);
	log_trace(logger, "CPU-MMU || Voy a ordenar la tlb por tiempo de carga");
	list_sort(tlb_proceso, _funcion_comparacion);
	log_trace(logger, "CPU-MMU || Voy a ordenar la tlb por tiempo de carga");
	mostrar_entradas(tlb_proceso);
	Entrada_TLB* removido = (Entrada_TLB*)list_remove(tlb_proceso, 0);
	log_info(logger, "CPU-MMU Pagina removida: %d",removido->numero_pagina);

	log_trace(logger, "CPU-MMU || Se agrega la entrada a la TLB");
	list_add(tlb_proceso, entrada);
}

void set_numero_pagina(Datos_calculo_direccion* datos, uint32_t direccion_logica){
	if (datos->tamano_pagina != 0){
		log_trace(logger, "DIRECCION LOGICA: %d||\tTAMANO DE PAGINA: %d",direccion_logica, datos->tamano_pagina);
		datos->numero_pagina = floor(direccion_logica/datos->tamano_pagina);
		log_info(logger,"CPU-MMU NUMERO DE PAGINA Calculado: %d",datos->numero_pagina);
	}
	else
		log_error(logger, "CPU-MMU SE DIVIDE POR 0 :O");
}

void set_entrada_tabla_1er_nivel (Datos_calculo_direccion* datos){
	datos->entrada_tabla_primer_nivel = floor(datos->numero_pagina/datos->entradas_por_tabla);
	log_trace(logger, "CPU-MMU ENTRADA PRIMER NIVEL: %d", datos->entrada_tabla_primer_nivel);
}

void set_entrada_tabla_2do_nivel (Datos_calculo_direccion* datos){
	datos->entrada_tabla_segundo_nivel = datos->numero_pagina%datos->entradas_por_tabla;
	log_trace(logger, "CPU-MMU ENTRADA SEGUNDO NIVEL: %d", datos->entrada_tabla_segundo_nivel);
}

void set_desplazamiento (Datos_calculo_direccion* datos, uint32_t direccion_logica){
	datos->desplazamiento = direccion_logica - (datos->numero_pagina * datos->tamano_pagina);
	log_trace(logger, "CPU-MMU DESPLAZAMIENTO %d", datos->desplazamiento);
}

void calcular_datos_direccion(Datos_calculo_direccion* datos, uint32_t direccion_logica){
	set_numero_pagina(datos,direccion_logica);
	set_entrada_tabla_1er_nivel(datos);
	set_entrada_tabla_2do_nivel(datos);
	set_desplazamiento(datos,direccion_logica);
	mostrar_datos(datos);
}

Pagina_direccion* traducir_direccion(Datos_calculo_direccion* datos)
{
	uint32_t direccion_fisica;

	log_trace(logger, "CPU-MMU Inicio la traduccion");
	Pagina_direccion* resultado = malloc(sizeof(Pagina_direccion));
	resultado->marco = get_marco(datos);
	log_trace(logger, "CPU-MMU Obtengo el marco");
	resultado->numero_pagina = datos->numero_pagina;
	log_trace(logger, "CPU-MMU Seteo el numero de pagina");

	//	Calculo la direccion fisica
	//	( Marco * Tamaña Pagina ) + Desplazamiento
	log_trace(logger, "CPU-MMU ---- Valores previos al calculo de Direccion Fisica");
	log_trace(logger, "CPU-MMU ---- Marco: %d", resultado->marco);
	log_trace(logger, "CPU-MMU ---- Tamaño de Pagina: %d", datos->tamano_pagina);
	log_trace(logger, "CPU-MMU ---- Desplazamiento: %d", datos->desplazamiento);

	direccion_fisica = (resultado->marco * datos->tamano_pagina) + datos->desplazamiento;
	resultado->direccion_fisica = direccion_fisica;
	log_info(logger, "CPU-MMU Obtuve la DIRECCION FISICA: %d", direccion_fisica);
	return resultado;
}

uint32_t get_marco(Datos_calculo_direccion* datos){
	log_trace(logger, "CPU-MMU Empiezo a buscar el marco de la pagina %d", datos->numero_pagina);

	log_trace(logger, "CPU-MMU || Voy a buscar el marco en la tabla");
	uint32_t marco = buscar_marco(datos->numero_pagina);

	if(marco != -1){
		log_info(logger, "CPU-MMU || ENCONTRE el MARCO %d de la PAGINA %d y se encontraba en la TLB", marco, datos->numero_pagina);
		return marco;
	}else{
		marco = get_marco_memoria(datos);
		Entrada_TLB* nueva_entrada = malloc(sizeof(Entrada_TLB));
		nueva_entrada->numero_pagina = datos->numero_pagina;
		nueva_entrada->marco = marco;

		cargar_entrada(nueva_entrada);

		return marco;
	}
}

uint32_t solicitarValorEntrada(int conexionMemoria, uint32_t id_tabla, uint32_t numero_entrada, uint8_t codigo_operacion)
{
	uint32_t valorEntrada;

	//	Solicito el valor de la entrada a la Memoria
	send(conexionMemoria, &codigo_operacion, sizeof(uint8_t), 0);
	log_trace(logger, "CPU-MEMORIA-MMU Envio codigo de operacion %d", codigo_operacion);
	//	Envio el ID TABLA
	send(conexionMemoria, &id_tabla, sizeof(uint32_t), 0);
	log_trace(logger, "CPU-MEMORIA-MMU Envio ID de tabla: %d", id_tabla);
	//	Envio el Numero de Entrada
	send(conexionMemoria, &numero_entrada, sizeof(uint32_t), 0);
	log_trace(logger, "CPU-MEMORIA-MMU Envio numero de entrada: %d", numero_entrada);

	//	Recibo el Valor de la Entrada
	recv(conexionMemoria, &valorEntrada, sizeof(uint32_t), 0);
	log_info(logger, "CPU-MEMORIA-MMU: El valor de la entrada es: %d", valorEntrada);

	return valorEntrada;

}

uint32_t get_marco_memoria(Datos_calculo_direccion* datos)
{
	uint32_t id_tabla = datos->id_tabla_paginas1;
	uint32_t numeroEntradaPrimerNivel = datos->entrada_tabla_primer_nivel;
	uint32_t numeroEntradaSegundoNivel = datos->entrada_tabla_segundo_nivel;
	uint32_t id_tabla_paginas2;

	//	Solicito el ID de la tabla de paginas de segundo nivel
	id_tabla_paginas2=  solicitarValorEntrada(datos->conexion_memoria, id_tabla, numeroEntradaPrimerNivel, SOLICITAR_VALOR_ENTRADA1);

	//	Solicito el Marco
	uint32_t marco;
	marco=  solicitarValorEntrada(datos->conexion_memoria, id_tabla_paginas2, numeroEntradaSegundoNivel, SOLICITAR_VALOR_ENTRADA2);

	return marco;
}

void limpiar_tlb(t_list* tlb)
{
	void destruir_entradas(void* entrada){
		free(entrada);
	}

//	log_trace(logger, "CPU-MMU || Saco los elementos de la TLB");
	list_clean_and_destroy_elements(tlb,destruir_entradas);
	mostrar_entradas(tlb_proceso);
	log_trace(logger, "CPU-MMU || La TLB fue limpiada");
}

void mostrar_entradas(t_list* list){
	int aux = list_size(list);
	Entrada_TLB* entrada;

	for(int i=0;i<aux;i++){
		entrada= list_get(list,i);
		log_info(logger,"CPU-MMU || Numero de pagina: %d || Marco: %d || Tiempo de carga: %d || Ultima referencia: %d",
				entrada->numero_pagina,
				entrada->marco,
				entrada->tiempo_carga,
				entrada->ultima_referencia);
	}
}

void mostrar_datos(Datos_calculo_direccion* datos) {
	/*log_info(logger,"\n-----------DATOS PARA EL CALCULO DE DIRECCIONES----------------\n");
	log_info(logger,"CONEXION MEMORIA: %d\n", datos->conexion_memoria);
	log_info(logger,"ID TABLA PRIMER NIVEL: %d\n", datos->id_tabla_paginas1);
	log_info(logger,"DESPLAZAMIENTO: %d\n", datos->desplazamiento);
	log_info(logger,"ENTRADA PRIMER NIVEL: %d\n", datos->entrada_tabla_primer_nivel);
	log_info(logger,"ENTRADA SEGUNDO NIVEL: %d\n", datos->entrada_tabla_segundo_nivel);
	log_info(logger,"NUMERO PAGINA: %d\n", datos->numero_pagina);
	log_info(logger,"ENTRADAS POR TABLA: %d\n", datos->entradas_por_tabla);
	log_info(logger,"TAMAÑO PAGINA: %d\n", datos->tamano_pagina);
	*/
}
//
//void crear_tabla_prueba(){
//	Entrada_TLB* entrada1 = malloc(sizeof(Entrada_TLB));
//	entrada1->numero_pagina = 0;
//	entrada1->marco = 0;
//	entrada1->tiempo_carga = time(NULL);
//	entrada1->ultima_referencia = 0;
//
//	Entrada_TLB* entrada2 = malloc(sizeof(Entrada_TLB));
//	entrada2->numero_pagina = 1;
//	entrada2->marco = 1;
//	entrada2->tiempo_carga = time(NULL);
//	entrada2->ultima_referencia = 0;
//
//	Entrada_TLB* entrada3 = malloc(sizeof(Entrada_TLB));
//	entrada3->numero_pagina = 2;
//	entrada3->marco = 2;
//	entrada3->tiempo_carga = time(NULL);
//	entrada3->ultima_referencia = 0;
//
//	Entrada_TLB* entrada4 = malloc(sizeof(Entrada_TLB));
//	entrada4->numero_pagina = 3;
//	entrada4->marco = 3;
//	entrada4->tiempo_carga = time(NULL);
//	entrada4->ultima_referencia = 0;
//
//	cargar_entrada(entrada1);
//	sleep(1);
//	cargar_entrada(entrada2);
//	sleep(1);
//	cargar_entrada(entrada3);
//	cargar_entrada(entrada4);
//}
