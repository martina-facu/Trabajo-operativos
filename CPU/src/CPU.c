#include "utils.h"

uint32_t interrupcion = 0;

void ejecutar_ciclo_instrucciones(Pcb* pcb,t_config* config, uint32_t* flag) {
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == 4) {
		requiere_fetch_operands = true;
	}

	//fetch_operands
	if (requiere_fetch_operands) {
		// TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy, va a ser una llamada a memoria
		printf("\nES UNA INSTRUCCION COPY");
		printf("\n");
	}

	*flag = execute(instruccion,config);

	if (*flag == 0) {
		*flag = interrupcion;
	}
}

int main(void) {
	t_config* config = config_create("cpu.config");
//	int socket_dispatch = 0;
//	int socket_interrupt = 0;

	int conexion_memoria = levantar_conexion_memoria(config);
//	int kernel_dispatch = levantar_canal_dispatch(config, &socket_dispatch);
//	int kernel_interrupt = levantar_canal_dispatch(config, &socket_interrupt);

//	Dispatch
	char* puerto_dispatch = config_get_string_value(config,
			"PUERTO_ESCUCHA_DISPATCH");
	printf("\nPuerto escucha %s", puerto_dispatch);

	int socket_dispatch = iniciar_servidor("127.0.0.1", puerto_dispatch);
	printf("\nSocket %d", socket_dispatch);

	int kernel_dispatch = esperar_cliente(socket_dispatch);
	printf("\nCliente: %d", kernel_dispatch);
	printf("\n");

	uint8_t mensaje = 0;
	recv(kernel_dispatch, &mensaje, sizeof(uint8_t), 0);
	printf("Mensaje recibido dispatch: %d", mensaje);
	printf("\n");

	uint8_t handshake = 4;

	send(kernel_dispatch, &handshake, sizeof(uint8_t), 0);

//	Interrupt
//	char* puerto_interrupt = config_get_string_value(config,
//			"PUERTO_ESCUCHA_INTERRUPT");
//	printf("\nPuerto interrupt %s", puerto_interrupt);
//
//	int socket_interrupt = iniciar_servidor("127.0.0.1", puerto_interrupt);
//	printf("\nSocket %d", socket_interrupt);
//
//	int cliente2 = esperar_cliente(socket_interrupt);
//	printf("\nCliente: %d", cliente2);
//
//	uint8_t mensaje1 = 0;
//
//	recv(cliente2, &mensaje1, sizeof(uint8_t), 0);
//	printf("\nMensaje recibido interrupt: %d", mensaje1);
//	printf("\n");
//
//	uint8_t handshake1 = 5;
//	send(cliente2, &handshake1, sizeof(uint8_t), 0);

//Recibir pcb de kernel

	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
	pcb_mostrar(pcb);

//	Ejecutar ciclo de instrucciones
	uint32_t flag = 0;
	while (flag == 0) {
		ejecutar_ciclo_instrucciones(pcb,config,&flag);
	}

//	DEVOLVER PCB AL KERNEL
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));

	if(flag == 1){
		void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
		send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);

	}else{
		uint32_t* tiempo_bloqueo = malloc(sizeof(uint32_t));
		*tiempo_bloqueo = flag;

		void* a_enviar = pcb_serializar(pcb,tamano_mensaje,2);
		memcpy(a_enviar + *tamano_mensaje, tiempo_bloqueo, sizeof(uint32_t));

		*tamano_mensaje = *tamano_mensaje + sizeof(uint32_t);

		send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);
	}


	close(socket_dispatch);
//	close(socket_interrupt);
	close(conexion_memoria);
	return EXIT_SUCCESS;
}
