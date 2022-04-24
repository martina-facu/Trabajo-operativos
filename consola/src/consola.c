#include <stdio.h>
#include <stdlib.h>
#include "cliente.h"

void exit_gracefully(int return_nr) {
  exit(return_nr);
}

int main(int argc, char** argv) {

	//Conecta como cliente al Kernel
	int sockfd = conectar_kernel();
	saludo_inicial_kernel(sockfd);
	close(sockfd);
	return EXIT_SUCCESS;
}
