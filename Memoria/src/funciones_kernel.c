
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
		uint8_t operacion;
		recv(socket_kernel,&operacion,sizeof(uint8_t),0);
		log_trace(logger,"RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);
		switch(operacion){
		case INICIALIZAR_PROCESO:
			inicializar_proceso();
			break;
		}
	}
}

void inicializar_proceso(){
	uint32_t pid;
	uint32_t tam_proceso;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);
	log_trace(logger,"PID RECIBIDO: %d",pid);
	recv(socket_kernel,&tam_proceso,sizeof(uint32_t),0);
	log_trace(logger,"TAM PROCESO RECIBIDO: %d",tam_proceso);
	t_proceso *proceso = crear_proceso(pid,tam_proceso);
	log_trace(logger,"PROCESO CREADO CON EXITO, MANDANDO MENSAJE...");
	send(socket_kernel,&proceso->entrada_1,sizeof(uint32_t),0);
}

t_proceso* crear_proceso(uint32_t pid, uint32_t tam_proceso){
	log_trace(logger,"CREANDO PROCESO");
	uint32_t cant_pag = division_entera(tam_proceso,TAM_PAGINA);
	log_trace(logger,"CANTIDAD PAGINAS: %d", cant_pag);
	uint32_t cant_entradas_1 = division_entera(cant_pag,ENTRADAS_POR_TABLA);
	log_trace(logger,"CANTIDAD ENTRADAS: %d", cant_entradas_1);

	if(cant_entradas_1>ENTRADAS_POR_TABLA){
		log_error(logger,"NO SE DEBERIA PODER, QUE HACEMOS?");
	}

	t_tabla_1* tabla = malloc(sizeof(t_tabla_1));
	inicializar_tabla_1(tabla,pid);
	asignar_tabla_2(tabla,cant_entradas_1);
	list_add(tabla_1_l,tabla);

	uint32_t entrada = list_size(tabla_1_l)-1;
	log_trace(logger,"ENTRADA: %d || PID: %d",entrada,pid);

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
	log_trace(logger,"INICIALIZANDO TABLA PRIMER NIVEL || PID: %d", pid);
	tabla->pid = pid;
	tabla->entradas = malloc(ENTRADAS_POR_TABLA* sizeof(int));
	for(int i=0; i<ENTRADAS_POR_TABLA;i++){
		*(tabla->entradas + i)= -1;
		log_trace(logger,"NUMERO: %d",*(tabla->entradas+i));
	}
}

void asignar_tabla_2(t_tabla_1* tabla,uint32_t cant_entradas_1){
	for(int i=0;i<cant_entradas_1;i++){
		t_tabla_2* tabla2= malloc(sizeof(t_tabla_2));
		tabla2->entradas=list_create();
		log_trace(logger,"ASIGNADO TABLA SEGUNDO NIVEL || NUMERO: %d", i);
		inicializar_tabla_2(tabla2->entradas);
		log_trace(logger,"TABLA INICIALIZADA");

		list_add(tabla_2_l,tabla2);

		log_trace(logger,"TABLA AGREGADA");

		*(tabla->entradas+i)=list_size(tabla_2_l)-1;
		log_trace(logger,"NUMERO: %d",*(tabla->entradas+i));
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
		log_trace(logger,"INICIALIZANDO ENTRADA DE SEGUNDO NIVEL NUMERO: %d", i);
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


























