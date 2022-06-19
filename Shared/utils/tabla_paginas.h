#ifndef TABLA_PAGINAS
#define TABLA_PAGINAS

#include <stdint.h>

typedef struct {
	uint32_t tamano_pagina;
	uint32_t entradas_por_tabla;
	uint32_t numero_pagina;
	uint32_t entrada_tabla_primer_nivel;
	uint32_t entrada_tabla_segundo_nivel;
	uint32_t desplazamiento;
} Datos_calculo_direccion;

void tabla_paginas_mostrar(Datos_calculo_direccion* datos);

#endif
