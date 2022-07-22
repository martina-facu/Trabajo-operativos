
#include "funciones_kernel.h"

//int obtener_y_ocupar_frame(){
//
//	int i = 0;
//	int frameEncontrado = 0;
//
//	//PONER COMO GLOBAL
//	int cantMarcosPpal = tamanoMemoria / tamanoPagina;
//
//	while (i < cantMarcosPpal && !frameEncontrado){
//
//		if(bitarray_test_bit(marcosOcupadosPpal, i) == 0){
//
//			//ENCUENTRO UN MARCO DISPONBILE
//			bitarray_set_bit(marcosOcupadosPpal, 1); //LO SELECCIONO COMO NO DISPONBILE, VER QUE VALER PONE TIENE QUE QUEDAR 1
//			frameEncontrado = 1;
//			framesLibres--;
//		}
//		else // DE LO CONTRRAIO SIGO BUSCANDO
//			i++;
//
//	}
//
//	return i;
//}
void* funciones_kernel(){
	while(1){
		log_info(logger,"MEMORIA-KERNEL || TE ESTAMOS ESPERANDO KERNELITO");
		uint8_t operacion;
		recv(socket_kernel,&operacion,sizeof(uint8_t),0);
		log_trace(logger,"MEMORIA-KERNEL || RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);
		switch(operacion){
		case INICIALIZAR_PROCESO:
			inicializar_proceso();
			break;
		case SUSPENDER_PROCESO:
			suspender_proceso();
			break;
		case FINALIZAR_PROCESO:
			finalizar_proceso();
			break;
		default:
			log_error(logger, "MEMORIA-KERNEL || El mensaje recibido %d no corresponde a uno de los conocidos.", operacion);
			log_error(logger, "MEMORIA-KERNEL || Se procede a cerrar la conexion");
			close(socket_kernel);
			break;
		}
	}
	return NULL;
}

void inicializar_proceso(){
	uint32_t pid;
	uint32_t tam_proceso;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);
	log_trace(logger,"MEMORIA-KERNEL || PID RECIBIDO: %d",pid);
	recv(socket_kernel,&tam_proceso,sizeof(uint32_t),0);
	log_trace(logger,"MEMORIA-KERNEL || TAM PROCESO RECIBIDO: %d",tam_proceso);
	t_proceso *proceso = crear_proceso(pid,tam_proceso);
	list_add(procesos,proceso);
	uint32_t cant_pag = division_entera(tam_proceso,TAM_PAGINA);
	crear_archivo_swap(pid,cant_pag);
	log_info(logger,"MEMORIA-KERNEL || PROCESO CREADO CON EXITO, MANDANDO MENSAJE...");
	send(socket_kernel,&proceso->entrada_1,sizeof(uint32_t),0);

	mostrar_tablas(proceso);
	mostrar_bitarray();
	// frame 1: 63 bytes - 127 bytes

//	t_tabla_1* tabla = list_get(tabla_1_l,proceso->entrada_1);
//	log_trace(logger,"HACEMOS UN GET DE LA TABLA 1, INDICE: %d ",proceso->entrada_1);
//	uint32_t indice = *(tabla->entradas);
//	log_trace(logger,"INDICE A LA SEGUNDA TABLA: %d ",indice);
//	t_tabla_2* tabla2 = list_get(tabla_2_l,indice);
//	log_trace(logger,"AGARRAMOS LA TABLA 2, Y ENTRADA 1");
//	t_entrada_2* entrada = list_get(tabla2->entradas,1);
//	entrada->bPres=1;
//	entrada->bMod=1;
//	entrada->frame=1;
//	t_memory_pag* pagina = malloc(sizeof(t_memory_pag));
//	pagina->entrada = entrada;
//	pagina->n_tabla_2=0;
//	pagina->n_entrada_2=1;
//	list_add(proceso->pagMem,pagina);
//	char aux[] = "holaa";
//	memcpy(memoria+entrada->frame*TAM_PAGINA+32,aux,strlen(aux)+1);


}

void crear_paginas_prueba(t_proceso* proceso){

//	for(int i = 0; i < 4; i++){
//		t_tabla_1* tabla = list_get(tabla_1_l, proceso->entrada_1);
//
//		uint32_t indice = *(tabla->entradas);
//
//		t_tabla_2* tabla2 = list_get(tabla_2_l,indice);
//
//		t_entrada_2* entrada = list_get(tabla2->entradas, i);
//		entrada->bPres = 1;
//		entrada->bUso = 1;
//		entrada->bMod = 1;
//		entrada->frame = i;
//		t_memory_pag* pagina = malloc(sizeof(t_memory_pag));
//		pagina->entrada = entrada;
//		pagina->n_tabla_2 = 0;
//		pagina->n_entrada_2 = i;
//		list_add(proceso->pagMem,pagina);
//		char aux[] = "holu";
//		memcpy(memoria+entrada->frame*TAM_PAGINA+10,aux,strlen(aux)+1);
//	}

//	printf("KERNEL || \nEntrada\t\tFrame\t\tBit Uso\t\tBit Mod\t\tBit Pres\n");
//
//	for (int i = 0; i < 4; i++){
//		t_tabla_2* tabla2 = list_get(tabla_2_l,0);
//		t_entrada_2* entrada = list_get(tabla2->entradas, i);
////		printf("MEMORIA-KERNEL || \n%d\t\t%d\t\t%d\t\t%d\t\t%d\n", i, entrada->frame, (int)entrada->bUso, (int)entrada->bMod, (int)entrada->bPres);
//
//	}
}

bool paginas_modificadas(void* entrada_){
	t_memory_pag* pagina= entrada_;
	return pagina->entrada->bMod ==1;
}
void mostrar_paginas(t_list* paginas){
	for(int i =0; i<list_size(paginas);i++){
		t_memory_pag* pagina = list_get(paginas,i);
		int num_pagina = pagina->n_tabla_2*ENTRADAS_POR_TABLA + pagina->n_entrada_2;
		log_info(logger,"MEMORIA-KERNEL || PAGINA: %d, FRAME: %d ",num_pagina,pagina->entrada->frame);
	}
}

void suspender_proceso(){
	uint32_t pid;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);

	t_proceso* proceso = list_get(procesos,pid);
	t_list* paginas_modificadas_proceso =list_filter(proceso->pagMem,paginas_modificadas);
	t_swap* paginas_a_swappear = malloc(sizeof(t_swap));
	paginas_a_swappear->pid = pid;
	paginas_a_swappear->memorias_a_swappear=  paginas_modificadas_proceso;
	list_add(pedidos_swap_l,paginas_a_swappear);
	sem_post(&s_swap);
	usleep(RETARDO_SWAP);
	while(!list_is_empty(proceso->pagMem)){
		t_memory_pag* pagina = list_remove(proceso->pagMem,0);
		pagina->entrada->bPres=0;
		bitarray_clean_bit(bitMem, pagina->entrada->frame);
	}
//	for(int i =0; i<list_size(paginas_modificadas_proceso);i++){
//		log_trace(logger,"ENTRAMOS EN EL FOR");
//		t_memory_pag* pagina = list_get(paginas_modificadas_proceso,i);
//		pagina->entrada->bPres=0;
//		int numero_pagina = pagina->n_tabla_2*ENTRADAS_POR_TABLA + pagina->n_entrada_2;
//		log_trace(logger,"VAMOS A SWAPEAR UNA PAGINA DEL FRAME: %d",pagina->entrada->frame);
//		swap_pagina(pid,numero_pagina,pagina->entrada);
//		log_trace(logger,"SALIMO DEL SWAP");
//		memset(memoria + pagina->entrada->frame*TAM_PAGINA, '\0', TAM_PAGINA);
//		log_trace(logger,"LIMPIAMOS MEMORIA");
//		bitarray_clean_bit(bitMem, pagina->entrada->frame);
//		log_trace(logger,"LIMPIAMOS EL BIT");
//	}
	mostrar_bitarray();
}

void finalizar_proceso(){

	uint32_t pid;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);

	log_trace(logger, "MEMORIA-KERNEL || Se recibe un pid %d para finalizar", pid);

	t_proceso* proceso = list_get(procesos,pid);
	log_trace(logger,"MEMORIA-KERNEL || AGARRAMOS EL PROCESO DE LA LISTA DE PROCESOS, PID: %d",proceso->pid);

	for(int i = 0; i < list_size(proceso->pagMem); i++){

		t_memory_pag* pagina = list_get(proceso->pagMem,i);
		log_trace(logger,"MEMORIA-KERNEL || VAMOS A Liberar UNA PAGINA DEL FRAME: %d",pagina->entrada->frame);
		memset(memoria + pagina->entrada->frame*TAM_PAGINA, '\0', TAM_PAGINA);
		bitarray_clean_bit(bitMem, pagina->entrada->frame);
		log_trace(logger, "MEMORIA-KERNEL || MUESTRO EL BITARRAY, LUEGO DE LIMPIAR ESE FRAME");
		mostrar_bitarray();
	}

	log_trace(logger, "MEMORIA-KERNEL || Ingresando a SWAP..");
	eliminar_archivo_swap(pid);
	log_trace(logger, "MEMORIA-KERNEL || Ingresando a MEMORIA..");

//TODO: Esto se me ocurrio para liberar memoria

//	log_trace(logger, "Eliminamos la lista de paginas presentes");
//	list_destroy(paginas_presentes_proceso);
//	log_trace(logger, "Eliminamos la lista de paginas en memoria y las liberamos");
//	list_destroy_and_destroy_elements(proceso->pagMem, free);
//	log_trace(logger, "Liberamos el proceso de la lista de procesos");
//	list_remove_and_destroy_element(procesos, proceso->pid, free);

}


t_proceso* crear_proceso(uint32_t pid, uint32_t tam_proceso){
	log_info(logger,"MEMORIA-KERNEL || CREANDO PROCESO");
	uint32_t cant_pag = division_entera(tam_proceso,TAM_PAGINA);
	log_info(logger,"MEMORIA-KERNEL || CANTIDAD PAGINAS: %d", cant_pag);
	uint32_t cant_entradas_1 = division_entera(cant_pag,ENTRADAS_POR_TABLA);
	log_info(logger,"MEMORIA-KERNEL || CANTIDAD ENTRADAS: %d", cant_entradas_1);

	if(cant_entradas_1>ENTRADAS_POR_TABLA){
		log_error(logger,"MEMORIA-KERNEL || NO SE DEBERIA PODER, QUE HACEMOS?");
	}

	t_tabla_1* tabla = malloc(sizeof(t_tabla_1));
	inicializar_tabla_1(tabla,pid);
	asignar_tabla_2(tabla,cant_entradas_1);
	list_add(tabla_1_l,tabla);

	uint32_t entrada = list_size(tabla_1_l)-1;
	log_info(logger,"MEMORIA-KERNEL || ENTRADA: %d || PID: %d",entrada,pid);

	t_proceso * proceso= malloc(sizeof(t_proceso));

	proceso->pid= pid;
	proceso->entrada_1= entrada;
	proceso->puntero=0;
	proceso->tam_proceso= tam_proceso;
	proceso->pagMem= list_create();
	proceso->contador=0;

	return proceso;
}

void inicializar_tabla_1(t_tabla_1* tabla,uint32_t pid){
	log_trace(logger,"KERNEL || INICIALIZANDO TABLA PRIMER NIVEL || PID: %d", pid);
	tabla->pid = pid;
	tabla->entradas = malloc(ENTRADAS_POR_TABLA* sizeof(int));
	for(int i=0; i<ENTRADAS_POR_TABLA;i++){
		*(tabla->entradas + i)= -1;
	}
}

void asignar_tabla_2(t_tabla_1* tabla,uint32_t cant_entradas_1){
	for(int i=0;i<cant_entradas_1;i++){
		t_tabla_2* tabla2= malloc(sizeof(t_tabla_2));
		tabla2->entradas=list_create();

		inicializar_tabla_2(tabla2->entradas);


		list_add(tabla_2_l,tabla2);



		*(tabla->entradas+i)=list_size(tabla_2_l)-1;

	}

}

void inicializar_tabla_2(t_list* entradas_tabla2){
	for(int i=0;i<ENTRADAS_POR_TABLA;i++){
		t_entrada_2* entrada = malloc(sizeof(t_entrada_2));
		entrada->frame=-1;
		entrada->bUso=0;
		entrada->bMod=0;
		entrada->bPres=0;
		list_add(entradas_tabla2,entrada);

	}
}

int division_entera(double numerador,  double denominador){

	double resultado,fractPart,intpart;

	resultado = (double) numerador / denominador;

	fractPart = modf(resultado, &intpart);

	if (fractPart > 0){
		return (int)intpart + 1;
	}
	else
		return (int)intpart;
}

void mostrar_bitarray(){
	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){
		log_trace(logger,"MEMORIA-BITARRAY: %d", (int)bitarray_test_bit(bitMem,i));
//				printf("%d", (int)bitarray_test_bit(bitMem,i));
		}
}


void mostrar_tablas(t_proceso* proceso_){
	t_tabla_1* tabla1 = list_get(tabla_1_l,proceso_->entrada_1);
	log_info(logger,"MEMORIA-ALGORITMO: MOSTRANDO TABLA 1, PERTENENCIENTE AL PROCESO: %d || ENTRADA DE TABLA 1 GLOBAL: %d",proceso_->pid,proceso_->entrada_1);
	for(int i=0; i < ENTRADAS_POR_TABLA && *(tabla1->entradas+i)!=-1 ;i++){
		uint32_t indice = *(tabla1->entradas+i);
		log_info(logger,"MEMORIA-ALGORITMO: MOSTRANDO TABLA 2, PERTENENCIENTE AL PROCESO: %d || INDICE DE TABLA 2 GLOBAL: %d ",proceso_->pid,indice);
		t_tabla_2* tabla2= list_get(tabla_2_l,indice);
		for(int j =0; j<ENTRADAS_POR_TABLA;j++){
			t_entrada_2* entrada2 = list_get(tabla2->entradas,j);
			log_info(logger,"MEMORIA-ALGORITMO: ENTRADA || U: %d || M: %d || P: %d || FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
		}
	}
}
