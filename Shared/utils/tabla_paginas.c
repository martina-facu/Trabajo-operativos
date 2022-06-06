#include "tabla_paginas.h"

void tabla_paginas_mostrar(Tabla_paginas* tabla) {
	printf("\n-----------TABLA DE PAGINAS----------------\n");
	printf("DESPLAZAMIENTO: %d\n", tabla->desplazamiento);
	printf("ENTRADA PRIMER NIVEL: %d\n", tabla->entrada_tabla_primer_nivel);
	printf("ENTRADA SEGUNDO NIVEL: %d\n", tabla->entrada_tabla_segundo_nivel);
	printf("NUMERO PAGINA: %d\n", tabla->numero_pagina);
}
