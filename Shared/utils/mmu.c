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

	Entrada_TLB* entrada = list_find(tlb_proceso,_coincide_pagina);

	if(entrada != NULL){
		entrada->ultima_referencia = time(NULL); //aca se actualiza porque se uso la pagina de la tlb
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
		list_add(tlb_proceso, entrada);
	}else{
		reemplazar_entrada(entrada);
	}

	log_info(logger, "CPU-TLB Se cargo una entrada a la tlb");
}

void reemplazar_entrada(Entrada_TLB* entrada){
	char* algoritmo = malloc(sizeof(char) * 4);
	strcpy(algoritmo, config_get_string_value(config_cpu, "REEMPLAZO_TLB"));

	if(string_contains(algoritmo,"FIFO")){
		reemplazar_entrada_FIFO(entrada);
	}else if(string_contains(algoritmo,"LRU")){
		reemplazar_entrada_LRU(entrada);
	}else{
		printf("OCURRIO UN ERROR EN DETECTAR EL ALGORITMO DE REEMPLAZO");
	}
	free(algoritmo);
}

void reemplazar_entrada_FIFO(Entrada_TLB* entrada){
	bool _funcion_comparacion(void* valor1, void* valor2){
		return ((Entrada_TLB*)valor1)->tiempo_carga < ((Entrada_TLB*)valor2)->tiempo_carga;
	}

	mostrar_entradas(tlb_proceso);
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

		if(tiempo1 == tiempo2){
			return ((Entrada_TLB*)valor1)->tiempo_carga < ((Entrada_TLB*)valor2)->tiempo_carga;
		}else{
			return tiempo1<tiempo2;
		}
	}

	mostrar_entradas(tlb_proceso);
	list_sort(tlb_proceso, _funcion_comparacion);
	mostrar_entradas(tlb_proceso);
	Entrada_TLB* removido = (Entrada_TLB*)list_remove(tlb_proceso, 0);
	log_info(logger, "CPU-MMU Pagina removida: %d",removido->numero_pagina);

	list_add(tlb_proceso, entrada);
}

void set_numero_pagina(Datos_calculo_direccion* datos, uint32_t direccion_logica){


	if (datos->tamano_pagina != 0){
		log_trace(logger, "CALCULO DE PAGINA");
		log_trace(logger, "DIRECCION LOGICA: %d, TAMANO DE PAGINA: %d",direccion_logica, datos->tamano_pagina);


		datos->numero_pagina = floor(direccion_logica/datos->tamano_pagina);

		log_trace(logger,"NUMERO DE PAGINA: %d",datos->numero_pagina);
	}

	else
		log_info(logger, "CPU: SE DIVIDE POR 0 :O");
}

void set_entrada_tabla_1er_nivel (Datos_calculo_direccion* datos){
	log_trace(logger, "NUMERO DE PAGINA: %d, ENTRADAS POR TABLA: %d", datos->numero_pagina,datos->entradas_por_tabla);
	datos->entrada_tabla_primer_nivel = floor(datos->numero_pagina/datos->entradas_por_tabla);
	log_trace(logger, "ENTRADA: %d", datos->entrada_tabla_primer_nivel);
}

void set_entrada_tabla_2do_nivel (Datos_calculo_direccion* datos){
	datos->entrada_tabla_segundo_nivel = datos->numero_pagina%datos->entradas_por_tabla;
}

void set_desplazamiento (Datos_calculo_direccion* datos, uint32_t direccion_logica){
	datos->desplazamiento = direccion_logica - (datos->numero_pagina * datos->tamano_pagina);
}

void calcular_datos_direccion(Datos_calculo_direccion* datos, uint32_t direccion_logica){
	set_numero_pagina(datos,direccion_logica);
	set_entrada_tabla_1er_nivel(datos);
	set_entrada_tabla_2do_nivel(datos);
	set_desplazamiento(datos,direccion_logica);
//	mostrar_datos(datos);
}

Pagina_direccion* traducir_direccion(Datos_calculo_direccion* datos)
{
	uint32_t direccion_fisica;

	log_info(logger, "CPU-MMU Inicio la traduccion");
	Pagina_direccion* resultado = malloc(sizeof(Pagina_direccion));
	resultado->marco = get_marco_memoria(datos);
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
//	resultado->direccion_fisica = (resultado->marco * datos->tamano_pagina) + datos->desplazamiento;
	log_trace(logger, "CPU-MMU Seteo la direccion fisica");
	return resultado;
}

uint32_t get_marco(Datos_calculo_direccion* datos){
	log_info(logger, "CPU-MMU Empiezo a buscar el marco de la pagina %d", datos->numero_pagina);

	uint32_t marco = buscar_marco(datos->numero_pagina);

	if(marco != -1){
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

//uint32_t get_marco_memoria(Datos_calculo_direccion* datos)
//{
//
//	uint8_t codigo_operacion;
//	int conexion = datos->conexion_memoria;
//
//	// Busco la entrada de la tabla de paginas de segundo nivel
//	codigo_operacion = SOLICITAR_VALOR_ENTRADA1;
//
//	uint32_t* id_tabla_paginas2 = malloc(sizeof(uint32_t));
//
//	uint32_t id_tabla = datos->id_tabla_paginas1;
//	uint32_t numero_entrada = datos->entrada_tabla_primer_nivel;
//	log_info(logger, "ENVIAMOS || CODIGO: %d || ID_ TABLA: %d || NUM ENTRADA: %d", codigo_operacion,id_tabla,numero_entrada);
//	send(conexion, &codigo_operacion, sizeof(uint8_t), 0);
//	log_trace(logger, "CPU-MEMORIA Envio codigo de operacion SOLICITAR_VALOR_ENTRADA1");
//	send(conexion, &id_tabla, sizeof(uint32_t), 0);
//	log_trace(logger, "CPU-MEMORIA Envio Id de tabla");
//	send(conexion, &numero_entrada, sizeof(uint32_t), 0);
//	log_trace(logger, "CPU-MEMORIA Envio numero de entrada");
//
//	recv(conexion, id_tabla_paginas2, sizeof(uint32_t), 0);
//	log_info(logger, "RECIBO DE SUELDO: %d", id_tabla_paginas2);
////	Busco la entrada de la tabla de paginas de segundo nivel
//	codigo_operacion = SOLICITAR_VALOR_ENTRADA2;
//
//	id_tabla = *id_tabla_paginas2;
//	numero_entrada = datos->entrada_tabla_segundo_nivel;
//	log_info(logger, "ENVIAMOS || CODIGO: %d || ID_ TABLA: %d || NUM ENTRADA: %d || NUM PAG: %d", codigo_operacion,id_tabla,numero_entrada,datos->numero_pagina);
//	send(conexion, &codigo_operacion, sizeof(uint8_t), 0);
//	send(conexion, &id_tabla, sizeof(uint32_t), 0);
//	send(conexion, &numero_entrada, sizeof(uint32_t), 0);
//	send(conexion, &(datos->numero_pagina), sizeof(uint32_t), 0);
//
//	uint32_t* marco = malloc(sizeof(uint32_t));
//	recv(conexion, marco, sizeof(uint32_t), 0);
//
//	log_info(logger, "RECIBIMOS MARCO: %d" ,marco);
//
//	uint32_t aux = *marco;
//
//	free(marco);
//	free(id_tabla_paginas2);
//	return aux;
//}

uint32_t solicitarValorEntrada(int conexionMemoria, uint32_t id_tabla, uint32_t numero_entrada, uint8_t codigo_operacion)
{
	uint32_t valorEntrada;

	//	Solicito el valor de la entrada a la Memoria
	send(conexionMemoria, &codigo_operacion, sizeof(uint8_t), 0);
	log_trace(logger, "CPU-MEMORIA-MMU Envio codigo de operacion SOLICITAR_VALOR_ENTRADA1");
	//	Envio el ID TABLA
	send(conexionMemoria, &id_tabla, sizeof(uint32_t), 0);
	log_trace(logger, "CPU-MEMORIA-MMU Envio ID de tabla: %d", id_tabla);
	//	Envio el Numero de Entrada
	send(conexionMemoria, &numero_entrada, sizeof(uint32_t), 0);
	log_trace(logger, "CPU-MEMORIA-MMU Envio numero de entrada: %d", numero_entrada);

	//	Recibo el Valor de la Entrada
	recv(conexionMemoria, &valorEntrada, sizeof(uint32_t), 0);
	log_info(logger, "CPU-MEMORIA-MMU: El valor de la primera entrada es: %d", valorEntrada);

	return valorEntrada;

}

uint32_t get_marco_memoria(Datos_calculo_direccion* datos)
{
	uint32_t id_tabla = datos->id_tabla_paginas1;
	uint32_t numeroEntradaPrimerNivel = datos->entrada_tabla_primer_nivel;
	uint32_t numeroEntradaSegundoNivel = datos->entrada_tabla_segundo_nivel;
	uint32_t id_tabla_paginas2;

//	uint32_t* id_tabla_paginas2 = malloc(sizeof(uint32_t));

//	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));
//	coordenada->id_tabla = datos->id_tabla_paginas1;
//	coordenada->numero_entrada = datos->entrada_tabla_primer_nivel;

	//	Solicito el ID de la tabla de paginas de segundo nivel
	id_tabla_paginas2=  solicitarValorEntrada(datos->conexion_memoria, id_tabla, numeroEntradaPrimerNivel, SOLICITAR_VALOR_ENTRADA1);
//	enviar_coordenada(coordenada, id_tabla_paginas2, datos->conexion_memoria,SOLICITAR_VALOR_ENTRADA1);
//
//	coordenada->id_tabla = *id_tabla_paginas2;
//	coordenada->numero_entrada = datos->entrada_tabla_segundo_nivel;

//	uint32_t* marco = malloc(sizeof(uint32_t));
//	enviar_coordenada(coordenada, marco, datos->conexion_memoria,SOLICITAR_VALOR_ENTRADA2);

	//	Solicito el Marco
	uint32_t marco;
	marco=  solicitarValorEntrada(datos->conexion_memoria, id_tabla_paginas2, numeroEntradaSegundoNivel, SOLICITAR_VALOR_ENTRADA2);


//	uint32_t aux = *marco;
//
//	free(marco);
//	free(id_tabla_paginas2);
//	return aux;
	return marco;
}

void limpiar_tlb(t_list* tlb)
{
	void destruir_entradas(void* entrada){
		free(entrada);
	}

	list_clean_and_destroy_elements(tlb,destruir_entradas);
}

void mostrar_entradas(t_list* list){
	int aux = list_size(list);
	Entrada_TLB* entrada;

	for(int i=0;i<aux;i++){
		entrada= list_get(list,i);
		log_info(logger,"\nNumero de pagina: %d Marco: %d Tiempo de carga: %d Ultima referencia: %d",
				entrada->numero_pagina,
				entrada->marco,
				entrada->tiempo_carga,
				entrada->ultima_referencia);
	}

	printf("\n");
}

void mostrar_datos(Datos_calculo_direccion* datos) {
	log_info(logger,"\n-----------DATOS PARA EL CALCULO DE DIRECCIONES----------------\n");
	log_info(logger,"CONEXION MEMORIA: %d\n", datos->conexion_memoria);
	log_info(logger,"ID TABLA PRIMER NIVEL: %d\n", datos->id_tabla_paginas1);
	log_info(logger,"DESPLAZAMIENTO: %d\n", datos->desplazamiento);
	log_info(logger,"ENTRADA PRIMER NIVEL: %d\n", datos->entrada_tabla_primer_nivel);
	log_info(logger,"ENTRADA SEGUNDO NIVEL: %d\n", datos->entrada_tabla_segundo_nivel);
	log_info(logger,"NUMERO PAGINA: %d\n", datos->numero_pagina);
	log_info(logger,"ENTRADAS POR TABLA: %d\n", datos->entradas_por_tabla);
	log_info(logger,"TAMAÑO PAGINA: %d\n", datos->tamano_pagina);
}

void crear_tabla_prueba(){
	Entrada_TLB* entrada1 = malloc(sizeof(Entrada_TLB));
	entrada1->numero_pagina = 0;
	entrada1->marco = 0;
	entrada1->tiempo_carga = time(NULL);
	entrada1->ultima_referencia = 0;

	Entrada_TLB* entrada2 = malloc(sizeof(Entrada_TLB));
	entrada2->numero_pagina = 1;
	entrada2->marco = 1;
	entrada2->tiempo_carga = time(NULL);
	entrada2->ultima_referencia = 0;

	Entrada_TLB* entrada3 = malloc(sizeof(Entrada_TLB));
	entrada3->numero_pagina = 2;
	entrada3->marco = 2;
	entrada3->tiempo_carga = time(NULL);
	entrada3->ultima_referencia = 0;

	Entrada_TLB* entrada4 = malloc(sizeof(Entrada_TLB));
	entrada4->numero_pagina = 3;
	entrada4->marco = 3;
	entrada4->tiempo_carga = time(NULL);
	entrada4->ultima_referencia = 0;

	cargar_entrada(entrada1);
	sleep(1);
	cargar_entrada(entrada2);
	sleep(1);
	cargar_entrada(entrada3);
	cargar_entrada(entrada4);
}
