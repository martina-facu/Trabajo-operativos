#ifndef MMU
#define MMU

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

typedef struct {
	uint32_t numero_pagina;
	uint32_t marco;
	time_t tiempo_carga;
	time_t ultima_referencia;
} Entrada_TLB;

typedef struct {
	uint32_t tamano_pagina;
	uint32_t entradas_por_tabla;
	uint32_t numero_pagina;
	uint32_t entrada_tabla_primer_nivel;
	uint32_t entrada_tabla_segundo_nivel;
	uint32_t desplazamiento;
} Datos_calculo_direccion;

void mostrar_datos(Datos_calculo_direccion* datos);

void cargar_entrada(t_config* config, t_list* tlb, Entrada_TLB* entrada);

uint32_t buscar_marco(uint32_t* pagina, t_list* tlb);

void set_numero_pagina(Datos_calculo_direccion* datos, double direccion_logica);

void set_entrada_tabla_1er_nivel(Datos_calculo_direccion* datos);

void set_entrada_tabla_2do_nivel(Datos_calculo_direccion* datos);

void set_desplazamiento(Datos_calculo_direccion* datos, double direccion_logica);

void calcular_datos_direccion(Datos_calculo_direccion* datos,
		double direccion_logica);

void reemplazar_entrada_FIFO(t_list* tlb, Entrada_TLB* entrada);

void reemplazar_entrada_LRU(t_list* tlb, Entrada_TLB* entrada);

void mostrar_datos(Datos_calculo_direccion* datos);

void mostrar_entradas(t_list* list);

t_list* crear_tabla_prueba();


#endif
