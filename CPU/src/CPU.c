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

	//execute
	*flag = execute(instruccion,config,pcb);

	//check interrupt
	if (*flag == 0) {
		*flag = interrupcion;
	}
}

int main(void) {
	t_config* config = config_create("cpu.config");
	int socket_dispatch = 0;
	int socket_interrupt = 0;

	int conexion_memoria = levantar_conexion_memoria(config);
	int kernel_dispatch = levantar_canal_dispatch(config, &socket_dispatch);
	int kernel_interrupt = levantar_puerto_interrupt(config, &socket_interrupt);

//	Recibir pcb del kernel
	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
	pcb_mostrar(pcb);

//	Ejecutar ciclo de instrucciones
	uint32_t devolver_pcb = 0;
	while (devolver_pcb == 0) {
		ejecutar_ciclo_instrucciones(pcb,config,&devolver_pcb);
	}

	pcb_mostrar(pcb);
//	DEVOLVER PCB AL KERNEL
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
	void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
	send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);

//	Cierro conexiones
	close(conexion_memoria);
	close(socket_dispatch);
	close(socket_interrupt);
	return EXIT_SUCCESS;
}
