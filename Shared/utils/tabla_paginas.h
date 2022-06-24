#ifndef TABLA_PAGINAS
#define TABLA_PAGINAS

#include <stdint.h>
#include <commons/log.h>

typedef struct {
   uint32_t numero_pagina;
   uint32_t entrada_tabla_primer_nivel;
   uint32_t entrada_tabla_segundo_nivel;
   uint32_t desplazamiento;
} Tabla_paginas;

void tabla_paginas_mostrar(Tabla_paginas* tabla, t_log* logger);

#endif
