#include "tabla_paginas.h"

void tabla_paginas_mostrar(Tabla_paginas* tabla, t_log* logger)
{
//	printf("\n-----------TABLA DE PAGINAS----------------\n");
	log_info(logger, "-----------TABLA DE PAGINAS----------------");
//	printf("DESPLAZAMIENTO: %d\n", tabla->desplazamiento);
	log_info(logger, "DESPLAZAMIENTO: %d", tabla->desplazamiento);
//	printf("ENTRADA PRIMER NIVEL: %d\n", tabla->entrada_tabla_primer_nivel);
	log_info(logger, "ENTRADA PRIMER NIVEL: %d", tabla->entrada_tabla_primer_nivel);
//	printf("ENTRADA SEGUNDO NIVEL: %d\n", tabla->entrada_tabla_segundo_nivel);
	log_info(logger, "ENTRADA SEGUNDO NIVEL: %d", tabla->entrada_tabla_segundo_nivel);
//	printf("NUMERO PAGINA: %d\n", tabla->numero_pagina);
	log_info(logger, "INUMERO PAGINA: %d", tabla->numero_pagina);
}
