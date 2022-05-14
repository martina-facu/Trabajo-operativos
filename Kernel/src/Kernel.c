/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include "./coneccion/consola/servidor/servidor.h"

//int main(void) {
//	printf("%s",string_duplicate("holaa")); /* prints !!!Hello World!!! */
//	return EXIT_SUCCESS;
//}

int main(void) {
	levantar_servidor_kernel();

}
