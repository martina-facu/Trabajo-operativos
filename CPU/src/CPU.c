#include "utils_CPU.h"

uint32_t interrupcion = 0;

void ejecutar_ciclo_instrucciones(Pcb* pcb,t_config* config, bool* devolver_pcb) {
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == COPY) {
		requiere_fetch_operands = true;
	}

	//fetch_operands
	if (requiere_fetch_operands) {
		// TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy, va a ser una llamada a memoria
		printf("\nES UNA INSTRUCCION COPY");
		printf("\n");
	}

	//execute
	*devolver_pcb = execute(instruccion,config,pcb);

	//check interrupt
	if (*devolver_pcb == false) {
		*devolver_pcb = interrupcion;
	}
}

int main(void) {
	t_config* config = config_create("cpu.config");
//	int socket_dispatch = 0;
//	int socket_interrupt = 0;
//	uint32_t cantidad_entradas, tamano_pagina = 0;
//
//	Iniciar conexiones
//	int conexion_memoria = levantar_conexion_memoria(config,&cantidad_entradas,&tamano_pagina);
//	int kernel_dispatch = levantar_canal_dispatch(config, &socket_dispatch);
//	int kernel_interrupt = levantar_puerto_interrupt(config, &socket_interrupt);

//	Manejo de TLB
	t_list* tlb = crear_tabla_prueba(config);
	uint32_t* pagina = malloc(sizeof(uint32_t));

	*pagina = 1;
	uint32_t marco =  buscar_marco(*pagina,tlb);
	printf("El marco de la pagina %d es: %d",*pagina, marco);

	sleep(1);//espero para que tengan tiempo de referencia diferente

	*pagina = 0;
	marco =  buscar_marco(*pagina,tlb);
	printf("\nEl marco de la pagina %d es: %d\n",*pagina, marco);

	Entrada_TLB* entrada = malloc(sizeof(Entrada_TLB));
	entrada->numero_pagina = 2;
	entrada->marco = 0;

	cargar_entrada(config, tlb, entrada);
	mostrar_entradas(tlb);

	Datos_calculo_direccion* datos = malloc(sizeof(Datos_calculo_direccion));
	datos->id_tabla_paginas1 = 5;
	datos->conexion_memoria = 3;
	datos->entradas_por_tabla = 4;
	datos->tamano_pagina = 100;

	calcular_datos_direccion(datos, 50003);

////	Recibir pcb del kernel
//	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
//	pcb_mostrar(pcb);
//
////	Ejecutar ciclo de instrucciones
//	bool devolver_pcb = false;
//	while (devolver_pcb == false) {
//		ejecutar_ciclo_instrucciones(pcb,config,&devolver_pcb);
//	}
//
//	pcb_mostrar(pcb);
//
////	DEVOLVER PCB AL KERNEL
//	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
//	void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
//	send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);
//
////	Cierro conexiones
//	close(conexion_memoria);
//	close(socket_dispatch);
//	close(socket_interrupt);
	return EXIT_SUCCESS;
}
