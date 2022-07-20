/*
 * mmu.h
 *
 *  Created on: 4 jul. 2022
 *      Author: utnso
 */

#ifndef UTILS_MMU_H_
#define UTILS_MMU_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <time.h>
#include "../conexion.h"

/*
 * Estructuras
*/

	typedef struct {
		uint32_t numero_pagina;
		uint32_t marco;
		time_t tiempo_carga;
		time_t ultima_referencia;
	} Entrada_TLB;

	typedef struct {
		uint32_t id_tabla_paginas1;
		uint32_t conexion_memoria;
		uint32_t tamano_pagina;
		uint32_t entradas_por_tabla;
		uint32_t numero_pagina;
		uint32_t entrada_tabla_primer_nivel;
		uint32_t entrada_tabla_segundo_nivel;
		uint32_t desplazamiento;
	} Datos_calculo_direccion;

	typedef struct {
		uint32_t numero_pagina;
		uint32_t marco;
		uint32_t direccion_fisica;
	} Pagina_direccion;

/*
 *	Variables Globales
*/

	Datos_calculo_direccion* datos;
	t_config* config_cpu;
	t_list* tlb_proceso;
	t_log* logger;

/*
 * Prototipo de funciones
*/

	void inicializar_mmu(t_config* config, t_list* tlb, t_log* logger_cpu);
	void mostrar_datos(Datos_calculo_direccion* datos);
	void cargar_entrada(Entrada_TLB* entrada);
	uint32_t buscar_marco(uint32_t pagina);
	void set_numero_pagina(Datos_calculo_direccion* datos, uint32_t direccion_logica);
	void set_entrada_tabla_1er_nivel(Datos_calculo_direccion* datos);
	void set_entrada_tabla_2do_nivel(Datos_calculo_direccion* datos);
	void set_desplazamiento(Datos_calculo_direccion* datos, uint32_t direccion_logica);
	void calcular_datos_direccion(Datos_calculo_direccion* datos, uint32_t direccion_logica);
	void reemplazar_entrada(Entrada_TLB* entrada);
	void reemplazar_entrada_FIFO(Entrada_TLB* entrada);
	void reemplazar_entrada_LRU(Entrada_TLB* entrada);
	uint32_t get_marco(Datos_calculo_direccion* datos);
	uint32_t get_marco_memoria(Datos_calculo_direccion* datos);
	Pagina_direccion* traducir_direccion(Datos_calculo_direccion* datos);
	void mostrar_datos(Datos_calculo_direccion* datos);
	void mostrar_entradas(t_list* list);
	void crear_tabla_prueba();
	void limpiar_tlb(t_list* tlb);
	uint32_t solicitarValorEntrada(int conexionMemoria, uint32_t id_tabla, uint32_t numero_entrada, uint8_t codigo_operacion);

#endif /* UTILS_MMU_H_ */
