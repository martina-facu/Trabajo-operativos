#include <stdint.h>

typedef struct {
   uint32_t numero_pagina;
   uint32_t entrada_tabla_primer_nivel;
   uint32_t entrada_tabla_segundo_nivel;
   uint32_t desplazamiento;
} Tabla_paginas;
