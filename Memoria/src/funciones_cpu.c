#include "funciones_cpu.h"


void* funciones_cpu(){
	
	while(1){
	
	uint8_t operacion;
	recv(socket_cpu,&operacion,sizeof(uint8_t),0);
	log_trace(logger,"RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);

	switch(operacion){

		case SOLICITAR_VALOR_ENTRADA1:
		break;
		case SOLICITAR_VALOR_ENTRADA2:
		break;
		case SOLICITAR_LECTURA:
		break;
		case SOLICITAR_ESCRITURA:
		break;

		default:
			log_error(logger, "El mensaje recibido %d no corresponde a uno de los conocidos.", operacion);
			log_error(logger, "Se procede a cerrar la conexion");
			close(socket_kernel);
			break;
		}
	
	}

	return NULL;
}
