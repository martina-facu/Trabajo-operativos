#include "utils_CPU.h"

bool interrupcion = false;

int main(void) {
	t_config* config = config_create("cpu.config");

	int socket_dispatch = 0;
	int socket_interrupt = 0;

//	Iniciar conexiones
	int conexion_memoria = levantar_conexion_memoria(config);
	inicializar_utils(config,conexion_memoria,&interrupcion);

	int kernel_dispatch = levantar_canal_dispatch(config, &socket_dispatch);
	int kernel_interrupt = levantar_puerto_interrupt(config, &socket_interrupt);

// ------------------- Test leer, escribir y copiar ----------------------------------------------------




////	Pruebas para manejo de TLB ---------------------------------------------
//	t_list* tlb = list_create();
//	inicializar_mmu(config,tlb);
//
//	crear_tabla_prueba();
//
//	uint32_t* pagina = malloc(sizeof(uint32_t));
//
//	*pagina = 1;
//	uint32_t marco =  buscar_marco(*pagina);
//	printf("El marco de la pagina %d es: %d",*pagina, marco);
//
//	sleep(1);//espero para que tengan tiempo de referencia diferente
//
//	*pagina = 0;
//	marco =  buscar_marco(*pagina);
//	printf("\nEl marco de la pagina %d es: %d\n",*pagina, marco);
//
//	Entrada_TLB* entrada = malloc(sizeof(Entrada_TLB));
//	entrada->numero_pagina = 2;
//	entrada->marco = 0;
//
//	cargar_entrada(entrada);
//	mostrar_entradas(tlb);
//
//	Datos_calculo_direccion* datos = malloc(sizeof(Datos_calculo_direccion));
//	datos->id_tabla_paginas1 = 5;
////	datos->conexion_memoria = conexion_memoria;
//	datos->conexion_memoria = 1;
//	datos->entradas_por_tabla = 4;
//	datos->tamano_pagina = 100;
//
//	calcular_datos_direccion(datos, 50003);
//
////	Pagina_direccion* resultado = malloc(sizeof(Pagina_direccion));
//	Pagina_direccion* resultado = traducir_direccion(datos);
//	mostrar_entradas(tlb);
////	--------------------------------------------------------------------------------------------------------------

//	Recibir pcb del kernel
	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
	pcb_mostrar(pcb);

//	Ejecutar ciclo de instrucciones
	bool devolver_pcb = false;
	while (devolver_pcb == false) {
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
