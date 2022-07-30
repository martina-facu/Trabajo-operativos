#include "funciones_cpu.h"

/*
 *  Funcion: 	funciones_cpu
 *  Entradas: 	void
 *  Salidas: 	void* (es la funcion de un hilo)
 *  Razon:		Evaluar los mensajes recibidos desde el CPU y enviar a procesar los mismos.
 *  Author: Grupo 99
 */

void* funciones_cpu(){
	
	while(1){
	
	uint8_t operacion;
	recv(socket_cpu,&operacion,sizeof(uint8_t),0);
	log_trace(logger,"MEMORIA-CPU || RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);

	switch(operacion){

		case SOLICITAR_VALOR_ENTRADA1:
			entrada1();
		break;
		case SOLICITAR_VALOR_ENTRADA2:
			entrada2();
		break;
		case SOLICITAR_LECTURA:
			lectura();
		break;
		case SOLICITAR_ESCRITURA:
			escritura();
		break;
		default:
			log_error(logger, "MEMORIA-CPU || El mensaje recibido %d no corresponde a uno de los conocidos.", operacion);
			log_error(logger, "MEMORIA-CPU || Se procede a cerrar la conexion");
//			Esto hay que descomentarlo una vez que se solucione el problema
//			close(socket_cpu);
			break;
		}

	}

	return NULL;
}

/*
 *  Funcion: 	entrada1
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Obtener el numero de entrada 1 solicitado y enviarselo a la CPU
 *  Author: Grupo 99
 */

void entrada1()
{
	uint32_t indice_tabla_1;
	recv(socket_cpu, &indice_tabla_1, sizeof(uint32_t),0);
	backup_indice_1= indice_tabla_1;
	log_trace(logger, "MEMORIA-CPU || INDICE DE TABLA 1 RECIBIDO: %d", indice_tabla_1);

	uint32_t entrada_tabla_1;
	recv(socket_cpu,&entrada_tabla_1,sizeof(uint32_t),0);
	log_trace(logger, "MEMORIA-CPU || ENTRADA DE TABLA 1 RECIBIDO: %d", entrada_tabla_1);
	backup_entrada_tabla_1 = entrada_tabla_1;

	t_tabla_1* tabla= list_get(tabla_1_l,indice_tabla_1);
	pid_ = tabla->pid;

	log_trace(logger, "MEMORIA-CPU || Obtengo el proceso %d de la lista de procesos", pid_);
	proceso_ = list_get(procesos,pid_);
	mostrar_tablas(proceso_);

	uint32_t indice_entrada_2 = *(tabla->entradas+entrada_tabla_1);
	log_info(logger, "MEMORIA-CPU || SE OBTUVO EL INDICE DE ENTRADA 2: %d", indice_entrada_2);
	retardoXcpu();
	send(socket_cpu,&indice_entrada_2,sizeof(uint32_t),0);
	log_trace(logger, "Se envio el mensaje a cpu con el indice de tabla 2");
}

/*
 *  Funcion: 	obtener_entrada
 *  Entradas: 	void
 *  Salidas: 	t_entrada_2*
 *  Razon:
 *  Author: Grupo 99
 */

t_entrada_2* obtener_entrada(uint32_t numero_pagina)
{
//	log_trace(logger,"-------------------------SIN NUMERO DE PAGINA--------------------------------");
//	log_info(logger, "--------------------OBTENER ENTRADA----------------------");
//	log_info(logger, "INDICE DE TABLA 1: %d", backup_indice_1);
//	t_tabla_1* tabla1 = list_get(tabla_1_l,backup_indice_1);
//	log_info(logger, "ENTRADA DE TABLA 1: %d", backup_entrada_tabla_1);
//	backup_indice_tabla_2= *(tabla1->entradas+backup_entrada_tabla_1);
//	log_info(logger, "INDICE DE TABLA 2: %d", backup_indice_tabla_2);
//	t_tabla_2* tabla2 = list_get(tabla_2_l,backup_indice_tabla_2);
//	log_info(logger, "ENTRADA DE TABLA 2: %d", backup_entrada_tabla_2);
//	t_entrada_2* entrada2= list_get(tabla2->entradas,backup_entrada_tabla_2);
//	log_info(logger, "MEMORIA-CPU || INDICE GLOBAL 1: %d\t||ENTRADA (INDICE TABLA 2): %d\t||ENTRADA TABLA 2: %d	", backup_indice_1,backup_indice_tabla_2,backup_entrada_tabla_2);
//	log_info(logger,"MEMORIA-CPU || ENTRADA\t||U: %d\t||M: %d\t||P: %d\t||FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);

	log_info(logger, "--------------------OBTENER ENTRADA----------------------");
	log_trace(logger,"-------------------------CON NUMERO DE PAGINA: %d--------------------------------",numero_pagina);
	log_info(logger, "--------------------OBTENER ENTRADA----------------------");
	log_info(logger, "INDICE DE TABLA 1: %d", backup_indice_1);
	t_tabla_1* tabla_1 = list_get(tabla_1_l,backup_indice_1);
	int entrada_1 = numero_pagina/ENTRADAS_POR_TABLA;
	log_info(logger, "ENTRADA DE TABLA 1: %d", entrada_1);
	int indice_tabla_2 = *(tabla_1->entradas+entrada_1);
	log_info(logger, "INDICE DE TABLA 2: %d", indice_tabla_2);
	t_tabla_2* tabla_2 = list_get(tabla_2_l,indice_tabla_2);
	int entrada_tabla_2= numero_pagina - entrada_1*ENTRADAS_POR_TABLA;
	log_info(logger, "ENTRADA DE TABLA 2: %d",entrada_tabla_2);
	t_entrada_2* entr2= list_get(tabla_2->entradas,entrada_tabla_2);

	log_info(logger, "MEMORIA-CPU || INDICE GLOBAL 1: %d\t||ENTRADA (INDICE TABLA 2): %d\t||ENTRADA TABLA 2: %d	",backup_indice_1,indice_tabla_2,entrada_tabla_2);
	log_info(logger,"MEMORIA-CPU || ENTRADA\t||U: %d\t||M: %d\t||P: %d\t||FRAME: %d", entr2->bUso,entr2->bMod,entr2->bPres,entr2->frame);

	return entr2;
}

/*
 *  Funcion: 	entrada1
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Obtener el numero de entrada 1 solicitado y enviarselo a la CPU
 *  Author: Grupo 99
 */

void entrada2(){
	uint32_t indice_tabla_2;
	recv(socket_cpu, &indice_tabla_2, sizeof(uint32_t),0);
	log_trace(logger, "MEMORIA-CPU || RECIBIMOS EL INDICE DE TABLA 2: %d\t||PID: %d", indice_tabla_2,pid_);
	uint32_t entrada_tabla_2;
	recv(socket_cpu,&entrada_tabla_2,sizeof(uint32_t),0);
	log_trace(logger, "MEMORIA-CPU || RECIBIMOS LA ENTRADA DE TABLA 2: %d\t||PID: %d", entrada_tabla_2,pid_);

	backup_entrada_tabla_2= entrada_tabla_2;
	t_tabla_2* tabla2= list_get(tabla_2_l,indice_tabla_2);
	t_entrada_2* entrada = list_get(tabla2->entradas,entrada_tabla_2);
	log_trace(logger, "MEMORIA-CPU || ENTRADA :%d\t||BIT USO: %d\t||BIT MOD: %d\t||BIT PRES: %d\t||FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);

	if(entrada->bPres==0){
		log_trace(logger, "MEMORIA-CPU El bit de presencia esta en 0, hacemos page fault");
		page_fault(entrada,indice_tabla_2,entrada_tabla_2);
	}

	log_info(logger, "MEMORIA-CPU || Se envia el nro de frame %d", entrada->frame);
	retardoXcpu();
	send(socket_cpu,&entrada->frame,sizeof(uint32_t),0);
}

/*
 *  Funcion: 	chequear_puntero
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:
 *  Author: Grupo 99
 */

void chequear_puntero(){
	if(list_size(proceso_->pagMem)==proceso_->puntero){
		proceso_->puntero=0;
	}
}

/*
 *  Funcion: 	page_fault
 *  Entradas: 	t_entrada_2* entrada
 *  			uint32_t indice_tabla_2
 *  			uint32_t entrada_tabla_2
 *  Salidas: 	void
 *  Razon:		Se levanta en memoria una pagina que no se encuentra en la misma
 *  Author: Grupo 99
 */

void page_fault(t_entrada_2* entrada,uint32_t indice_tabla_2, uint32_t entrada_tabla_2){
	log_trace(logger, "MEMORIA-CPU || SE CREA UNA PAGINA A AGREGAR");
	t_memory_pag *pagina= malloc(sizeof(t_memory_pag));
	pagina->n_tabla_2 = indice_tabla_2;
	pagina->n_entrada_2 = entrada_tabla_2;

	if(list_size(proceso_->pagMem)==MARCOS_POR_PROCESO || memoria_esta_llena()){
		chequear_puntero();
		log_info(logger, "MEMORIA-CPU || SE EJECUTA EL ALGORTIMO, SE VA A REEMPLAZAR UNA ENTRADA");
		ejecutar_algoritmo(entrada);
		entrada->bPres=1;
		entrada->bUso=1;
		entrada->bMod=0;
		log_trace(logger, "MEMORIA-CPU ||------- ENTRADA A AGREGAR O REEMPLAZAR ------- ");
		log_info(logger, "MEMORIA-CPU || ENTRADA :%d\t||BIT USO: %d\t||BIT MOD: %d\t||BIT PRES: %d\t||FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);
			//int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
		int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
		log_info(logger, "MEMORIA-CPU || ------ Nro PAGINA = %d ------ ", numero_pagina);
		traer_a_memoria(pid_,numero_pagina,entrada->frame);
		log_trace(logger, "MEMORIA-CPU || OPERACION DE PAGINA EXITOSA");
		log_info(logger, "MEMORIA-CPU || PID: %d\t||TABLA DE PAGINAS EN MEMORIA: ",pid_);
		mostrar_tabla_pagina();
		mostrar_bitarray();
		return;
	}else{
		log_trace(logger, "MEMORIA-CPU || SE AGREGA UNA PAGINA");
		int frame_libre = buscar_frame_libre();
		if(frame_libre<0){
			log_error(logger,"MEMORIA-CPU || HUBO ERROR AL EJECUTAR AL BUSCAR UN FRAME LIBRE");
		}
		proceso_->contador++;
		if(proceso_->contador >= MARCOS_POR_PROCESO){
			proceso_->puntero=proceso_->contador%(list_size(proceso_->pagMem)+1);
		}else{
			proceso_->puntero = proceso_->contador;
		}
		entrada->frame= frame_libre;
		log_info(logger, "MEMORIA-CPU || FRAME AGREGADO: %d", entrada->frame);
	}
	entrada->bPres=1;
	entrada->bUso=1;
	entrada->bMod=0;
	log_trace(logger, "MEMORIA-CPU ||ENTRADA A AGREGAR O REEMPLAZAR");
	log_trace(logger, "MEMORIA-CPU ||ENTRADA :%d\t||BIT USO: %d\t||BIT MOD: %d\t||BIT PRES: %d\t||FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);
	pagina->entrada = entrada;
	//int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
	int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
	log_info(logger, "MEMORIA-CPU || ------ Nro PAGINA = %d ------ ", numero_pagina);
	traer_a_memoria(pid_,numero_pagina,entrada->frame);
	list_add_sorted(proceso_->pagMem,pagina,ordenar);
	log_trace(logger, "MEMORIA-CPU || OPERACION DE PAGINA EXITOSA");
	log_info(logger, "MEMORIA-CPU || PID: %d\t||TABLA DE PAGINAS EN MEMORIA: ",pid_);
	mostrar_tabla_pagina();
	mostrar_bitarray();
}

/*
 *  Funcion: 	ordenar
 *  Entradas: 	void* entrada1
 *  			void* entrada2
 *  Salidas: 	bool
 *  Razon:
 *  Author: Grupo 99
 */

bool ordenar(void* entrada1, void* entrada2){

	t_memory_pag* entrada = entrada1;
	t_memory_pag* aux = entrada2;

	return entrada->entrada->frame < aux->entrada->frame;

}
/*
 *  Funcion: 	ejecutar_algoritmo
 *  Entradas: 	t_entrada_2* entrada
 *  Salidas: 	void
 *  Razon:		Ejecutar el algoritmo (CLOCK o CLOCK-M segun parametria) para reemplazo de pagina.
 *  Author: Grupo 99
 */

void ejecutar_algoritmo(t_entrada_2* entrada){
	if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK")==0){
		log_info(logger, "MEMORIA-CPU || ---------------- ALGORITMO CLOCK ----------------");
		clock_(entrada);
		}
	else if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK-M")==0){
		log_info(logger, "MEMORIA-CPU || ---------------- ALGORITMO CLOCK-M ----------------");
		clock_M(entrada);
	} else{
		log_error(logger,"MEMORIA-CPU || Error al leer el algoritmo a ejecutar. Se proceso a cerrar la memoria ya que no puedo procesar los page fault.");
		exit(EXIT_FAILURE);
	}
	log_info(logger,"MEMORIA-CPU || ----- TABLA DE PAGINAS DESPUES DE EJECUTAR EL ALGORITMO ----- ");
	mostrar_tabla_pagina();
}

/*
 *  Funcion: 	clock_
 *  Entradas: 	t_entrada_2* entrada
 *  Salidas: 	void
 *  Razon:		Ejecutar el algoritmo CLOCK para reemplazo de pagina.
 *  Author: Grupo 99
 */


void clock_(t_entrada_2* entrada){
	log_info(logger,"MEMORIA-CPU || ------ TABLA ANTES EJECUTAR ALGORITMO ------");
	log_info(logger,"MEMORIA-CPU || PUNTERO INICIAL: %d\t||CONTADOR: %d", proceso_->puntero, proceso_->contador);
	mostrar_tabla_pagina();
	log_info(logger,"MEMORIA-CPU || --------------- FIN  TABLA -----------------");
	while(1){
		log_trace(logger, "MEMORIA-CPU || BUSCO VICTIMA");
		t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);
		if(posible_victima->entrada->bUso==0){
			log_trace(logger, "MEMORIA-CPU || ENCONTRE LA VICTIMA");
			t_memory_pag* victima= posible_victima;
			// ANTES DE REEMPLAZAR LA ENTRADA
			victima->entrada->bPres=0;
			entrada->frame= posible_victima->entrada->frame;
			if(victima->entrada->bMod==1){
				log_trace(logger, "MEMORIA-CPU || El maquina tiene el bit de modificado en 1");
				t_swap* swap= malloc(sizeof(t_swap));
				log_trace(logger, "MEMORIA-CPU || CREO LA ESTRUCTURA SWAP, CON PID: %d",pid_);
				swap->pid=pid_;
				swap->memorias_a_swappear= list_create();
				log_trace(logger, "MEMORIA-CPU || Agrego la victima a la lista de paginas a swapear");
				list_add(swap->memorias_a_swappear,victima);
				log_trace(logger, "MEMORIA-CPU || AGREGUE LA VICTIMA A LA LISTA SWAP");
				log_trace(logger, "MEMORIA-CPU || PEDIDO AGREGADO");
				list_add(pedidos_swap_l,swap);
				sem_post(&s_swap);
			}

			victima->entrada = entrada;
			proceso_->contador++;
			log_trace(logger, "MEMORIA-CPU || CONTADOR: %d", proceso_->contador);
			proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem));
			log_trace(logger, "MEMORIA-CPU || PUNTERO: %d", proceso_->puntero);
			entrada->frame= posible_victima->entrada->frame;
//			return list_remove(proceso_->pagMem,proceso_->puntero);
			return;
		} else{
			posible_victima->entrada->bUso=0;
		}
		proceso_->contador++;
		log_trace(logger, "MEMORIA-CPU || CONTADOR: %d", proceso_->contador);
		proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
		log_trace(logger, "MEMORIA-CPU || PUNTERO: %d", proceso_->puntero);
	}
	log_info(logger,"MEMORIA-CPU || ------ TABLA DESPUES EJECUTAR ALGORITMO ------");
	log_info(logger,"MEMORIA-CPU || PUNTERO INICIAL: %d\t||CONTADOR: %d", proceso_->puntero, proceso_->contador);
	mostrar_tabla_pagina();
	log_info(logger,"MEMORIA-CPU || ----------------- FIN  TABLA -----------------");
}

/*
 *  Funcion: 	clock_M
 *  Entradas: 	t_entrada_2* entrada
 *  Salidas: 	void
 *  Razon:		Ejecutar el algoritmo CLOCK-M para reemplazo de pagina.
 *  Author: Grupo 99
 */
void clock_M(t_entrada_2* entrada){
	log_info(logger,"MEMORIA-CPU || ------ TABLA ANTES EJECUTAR ALGORITMO ------");
	log_trace(logger,"MEMORIA-CPU || PUNTERO INICIAL: %d\t||CONTADOR: %d", proceso_->puntero, proceso_->contador);
	mostrar_tabla_pagina();
	log_info(logger,"MEMORIA-CPU || --------------- FIN  TABLA -----------------");
	while(1){
		log_trace(logger, "MEMORIA-CPU || Obtengo una posible victima");
		for(int i = 0; i < list_size(proceso_->pagMem); i++){
			t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);

			log_trace(logger, "MEMORIA-CPU || Veo si tiene el buso y bmod en 0");

			if(posible_victima->entrada->bUso==0 && posible_victima->entrada->bMod==0){
				t_memory_pag* victima= posible_victima;
				// ANTES DE REEMPLAZAR LA ENTRADA
				victima->entrada->bPres=0;
				entrada->frame= posible_victima->entrada->frame;
				log_trace(logger, "MEMORIA-CPU || Tiene el b de usoy b de mod en 0, se hizo la vistima");
				victima->entrada = entrada;
				proceso_->contador++;
				log_trace(logger, "MEMORIA-CPU || Contador: %d", proceso_->contador);
				proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem));
				log_trace(logger, "MEMORIA-CPU || Puntero: %d", proceso_->puntero);
				entrada->frame= posible_victima->entrada->frame;
				log_trace(logger, "MEMORIA-CPU || Se asigno el frame %d", entrada->frame);
				return;
			}
			proceso_->contador++;
			proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);

		}
		mostrar_tabla_pagina();
		for(int i = 0; i < list_size(proceso_->pagMem); i++){
			t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);

			if (posible_victima->entrada->bUso==0 && posible_victima->entrada->bMod==1){
				t_memory_pag* victima= posible_victima;
				// ANTES DE REEMPLAZAR LA ENTRADA
				victima->entrada->bPres=0;
				entrada->frame= posible_victima->entrada->frame;
				log_trace(logger, "MEMORIA-CPU || Tiene el b de usoy b de mod en 1, se hizo la vistima");
				if(victima->entrada->bMod==1){
					log_trace(logger, "MEMORIA-CPU || El maquina tiene el bit de modificado en 1");
					t_swap* swap= malloc(sizeof(t_swap));
					log_trace(logger, "MEMORIA-CPU || CREO LA ESTRUCTURA SWAP, CON PID: %d",pid_);
					swap->pid=pid_;
					swap->memorias_a_swappear= list_create();
					log_trace(logger, "MEMORIA-CPU || Agrego la victima a la lista de paginas a swapear");
					list_add(swap->memorias_a_swappear,victima);
					log_trace(logger, "MEMORIA-CPU || AGREGUE LA VICTIMA A LA LISTA SWAP");
					log_trace(logger, "MEMORIA-CPU || PEDIDO AGREGADO");
					list_add(pedidos_swap_l,swap);
					sem_post(&s_swap);
				}
				victima->entrada = entrada;
				proceso_->contador++;
				proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem));
				entrada->frame= posible_victima->entrada->frame;
				return;
			}
			else{
				log_trace(logger, "MEMORIA-CPU || Pongo el b de uso en 0 ");
				posible_victima->entrada->bUso=0;
			}
			proceso_->contador++;
			proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
		}
	}
	log_info(logger,"MEMORIA-CPU || ------ TABLA DESPUES EJECUTAR ALGORITMO ------");
	log_info(logger,"MEMORIA-CPU || PUNTERO INICIAL: %d\t||CONTADOR: %d", proceso_->puntero, proceso_->contador);
	mostrar_tabla_pagina();
	log_info(logger,"MEMORIA-CPU || ----------------- FIN  TABLA -----------------");
}
/*
 *  Funcion: 	escritura
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Recibo una operacion de escritura desde la CPU y la proceso.
 *  Author: Grupo 99
 */
void escritura(){
	uint32_t direccion_fisica;
	recv(socket_cpu,&direccion_fisica,sizeof(uint32_t),0);
	uint32_t escritura;
	recv(socket_cpu,&escritura,sizeof(uint32_t),0);
	memcpy(memoria+direccion_fisica,&escritura,sizeof(uint32_t));
	uint32_t numero_pagina;
	retardoXcpu();
	send(socket_cpu,&escritura,sizeof(uint32_t),0);
	recv(socket_cpu,&numero_pagina,sizeof(uint32_t),0);
	t_entrada_2* entrada=  obtener_entrada(numero_pagina);
	entrada->bUso=1;
	entrada->bMod=1;
	log_info(logger, "MEMORIA-CPU || ------ ESCRITURA %d\t||FRAME: %d\t||DF: %d",escritura,entrada->frame, direccion_fisica );
}

/*
 *  Funcion: 	lectura
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Recibo una operacion de lectura desde la CPU y la proceso.
 *  Author: Grupo 99
 */

void lectura(){
	uint32_t direccion_fisica;
	recv(socket_cpu,&direccion_fisica,sizeof(uint32_t),0);
	uint32_t buffer;
	memcpy(&buffer,memoria+direccion_fisica,sizeof(uint32_t));
	uint32_t numero_pagina;
	retardoXcpu();
	send(socket_cpu,&buffer,sizeof(uint32_t),0);
	recv(socket_cpu,&numero_pagina,sizeof(uint32_t),0);
	t_entrada_2* entrada=  obtener_entrada(numero_pagina);
	entrada->bUso=1;
	log_info(logger, "MEMORIA-CPU || ------ LECTURA %d\t||FRAME: %d\t||DF: %d", buffer,entrada->frame, direccion_fisica);
}

/*
 *  Funcion: 	mostrar_tabla_pagina
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Muestro en los logs la tabla de paginas.
 *  Author: Grupo 99
 */

void mostrar_tabla_pagina(){
	for(int i=0; i < list_size(proceso_->pagMem); i++){
		t_memory_pag* pagina = list_get(proceso_->pagMem,i);
		t_entrada_2* entrada2 = pagina->entrada;
		log_info(logger,"MEMORIA-CPU || ENTRADA\t||U: %d\t||M: %d\t||P: %d\t||FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
	}
}
/*
 *  Funcion: 	mostrar_entrada
 *  Entradas: 	t_entrada_2* entrada2
 *  Salidas: 	void
 *  Razon:
 *  Author: Grupo 99
 */

void mostrar_entrada(t_entrada_2* entrada2){
	log_info(logger, "MEMORIA-CPU || INDICE GLOBAL 1: %d\t||ENTRADA (INDICE TABLA 2): %d\t||ENTRADA TABLA 2: %d	", backup_indice_1,backup_indice_tabla_2,backup_entrada_tabla_2);
	log_info(logger,"MEMORIA-CPU || ENTRADA\t||U: %d\t||M: %d\t||P: %d\t||FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
}

/*
 *  Funcion: 	memoria_esta_llena
 *  Entradas: 	void
 *  Salidas: 	bool
 *  Razon:
 *  Author: Grupo 99
 */

bool memoria_esta_llena(){
	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){
				if(0==(int)bitarray_test_bit(bitMem,i))
					return false;
		}
	return true;
}

/*
 *  Funcion: 	buscar_frame_libre
 *  Entradas: 	void
 *  Salidas: 	int
 *  Razon:		Buscar e informar el numero de frame libre
 *  Author: Grupo 99
 */

int buscar_frame_libre(){
	log_trace(logger, "CPU || FRAMES: %d",bitarray_get_max_bit(bitMem));
	mostrar_bitarray();
	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){
					if(!bitarray_test_bit(bitMem,i)){
						bitarray_set_bit(bitMem,i);
						return i;
					}
		}
	return -1;
}

/*
 *  Funcion: 	retardoXcpu
 *  Entradas: 	void
 *  Salidas: 	int
 *  Razon:		Retardo por operacion desde CPU. El valor del mismo se setea
 *  			por archivo de configuracion.
 *  Author: Grupo 99
 */

void retardoXcpu(){
	int retardoEnSegundos;

	retardoEnSegundos = RETARDO_MEMORIA/1000;
	sleep(retardoEnSegundos);
	log_info(logger, "MEMORIA-CPU || Se realizar el retardo de consumo de memoria por CPU de %d segundos", retardoEnSegundos);
}
