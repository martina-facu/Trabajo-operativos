#include "funciones_cpu.h"


void* funciones_cpu(){
	
	while(1){
	
	uint8_t operacion;
	recv(socket_cpu,&operacion,sizeof(uint8_t),0);
	log_trace(logger,"CPU || RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);

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
			log_error(logger, "CPU || El mensaje recibido %d no corresponde a uno de los conocidos.", operacion);
			log_error(logger, "CPU || Se procede a cerrar la conexion");
			close(socket_cpu);
			break;
		}

	}

	return NULL;
}

void entrada1(){

	log_trace(logger, "CPU || VAMOS A RECIBIR ENTRADA 1");
	uint32_t indice_tabla_1;
	recv(socket_cpu, &indice_tabla_1, sizeof(uint32_t),0);
	backup_indice_1= indice_tabla_1;
	log_trace(logger, "CPU || INDICE DE TABLA 1 RECIBIDO: %d", indice_tabla_1);

	uint32_t entrada_tabla_1;
	recv(socket_cpu,&entrada_tabla_1,sizeof(uint32_t),0);
	log_trace(logger, "CPU || ENTRADA DE TABLA 1 RECIBIDO: %d", entrada_tabla_1);
	backup_entrada_tabla_1 = entrada_tabla_1;

	t_tabla_1* tabla= list_get(tabla_1_l,indice_tabla_1);
	pid_ = tabla->pid;
	log_trace(logger, "CPU || PID DE TABLA: %d", pid_);

	log_trace(logger, "Obtengo el proceso %d de la lista de procesos", pid_);
	proceso_ = list_get(procesos,pid_);
	mostrar_tablas(proceso_);

	uint32_t indice_entrada_2 = *(tabla->entradas+entrada_tabla_1);
	log_trace(logger, "CPU || SE OBTUVO EL INDICE DE ENTRADA 2: %d", indice_entrada_2);

	send(socket_cpu,&indice_entrada_2,sizeof(uint32_t),0);
	log_trace(logger, "Se envio el mensaje a cpu");
}

t_entrada_2* obtener_entrada(){
	log_trace(logger, "--------------------OBTENER ENTRADA----------------------");
	log_trace(logger, "INDICE DE TABLA 1: %d", backup_indice_1);
	t_tabla_1* tabla1 = list_get(tabla_1_l,backup_indice_1);
	log_trace(logger, "ENTRADA DE TABLA 1: %d", backup_entrada_tabla_1);
	backup_indice_tabla_2= *(tabla1->entradas+backup_entrada_tabla_1);
	log_trace(logger, "INDICE DE TABLA 2: %d", backup_indice_tabla_2);
	t_tabla_2* tabla2 = list_get(tabla_2_l,backup_indice_tabla_2);
	log_trace(logger, "ENTRADA DE TABLA 2: %d", backup_entrada_tabla_2);
	t_entrada_2* entrada2= list_get(tabla2->entradas,backup_entrada_tabla_2);
	log_trace(logger, "CPU || INDICE GLOBAL 1: %d || ENTRADA (INDICE TABLA 2): %d || ENTRADA TABLA 2: %d	", backup_indice_1,backup_indice_tabla_2,backup_entrada_tabla_2);
	log_trace(logger,"CPU || ENTRADA || U: %d || M: %d || P: %d || FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
	return entrada2;
}
void entrada2(){
	log_trace(logger, "CPU || RECIBIMOS ENTRADA 2");

	uint32_t indice_tabla_2;
	recv(socket_cpu, &indice_tabla_2, sizeof(uint32_t),0);
	log_trace(logger, "CPU || RECIBIMOS EL INDICE DE TABLA 2: %d || PID: %d", indice_tabla_2,pid_);
	uint32_t entrada_tabla_2;
	recv(socket_cpu,&entrada_tabla_2,sizeof(uint32_t),0);
	log_trace(logger, "CPU || RECIBIMOS EL ENTRADA DE TABLA 2: %d || PID: %d", entrada_tabla_2,pid_);

	backup_entrada_tabla_2= entrada_tabla_2;
	t_tabla_2* tabla2= list_get(tabla_2_l,indice_tabla_2);
	t_entrada_2* entrada = list_get(tabla2->entradas,entrada_tabla_2);
	log_trace(logger, "ENTRADA :%d || BIT USO: %d || BIT MOD: %d || BIT PRES: %d || FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);

	if(entrada->bPres==0){
		log_trace(logger, "El bit de presencia esta en 0, hacemos page fault");
		page_fault(entrada,indice_tabla_2,entrada_tabla_2);
	}

	log_trace(logger, "Se envia el nro de frame %d", entrada->frame);
	send(socket_cpu,&entrada->frame,sizeof(uint32_t),0);
}

void chequear_puntero(){
	if(list_size(proceso_->pagMem)==proceso_->puntero){
		proceso_->puntero=0;
	}
}


void page_fault(t_entrada_2* entrada,uint32_t indice_tabla_2, uint32_t entrada_tabla_2){
	log_trace(logger, "CPU || SE CREA UNA PAGINA A AGREGAR");
	t_memory_pag *pagina= malloc(sizeof(t_memory_pag));
	pagina->n_tabla_2 = indice_tabla_2;
	pagina->n_entrada_2 = entrada_tabla_2;

	if(list_size(proceso_->pagMem)==MARCOS_POR_PROCESO || memoria_esta_llena()){
		chequear_puntero();
		log_trace(logger, "CPU || SE EJECUTA EL ALGORTIMO, SE VA A REEMPLAZAR UNA ENTRADA");
		ejecutar_algoritmo(entrada);
		entrada->bPres=1;
		entrada->bUso=1;
		entrada->bMod=0;
		log_trace(logger, "ENTRADA A AGREGAR O REEMPLAZAR");
		log_trace(logger, "ENTRADA :%d || BIT USO: %d || BIT MOD: %d || BIT PRES: %d || FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);
			//int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
		int numero_pagina = entrada_tabla_2*ENTRADAS_POR_TABLA +indice_tabla_2;
		log_trace(logger, "CPU || Nro pagina = %d", numero_pagina);
		log_trace(logger, "Nos vamo para swap");
		traer_a_memoria(pid_,numero_pagina,entrada->frame);
		log_trace(logger, "CPU || OPERACION DE PAGINA EXITOSA");
		log_trace(logger, "CPU || PID: %d ||TABLA DE PAGINAS EN MEMORIA: ",pid_);
		mostrar_tabla_pagina();
		mostrar_bitarray();
		return;
	}else{
		log_trace(logger, "CPU || SE AGREGA UNA PAGINA");
		int frame_libre = buscar_frame_libre();
		if(frame_libre<0){
			log_trace(logger,"HUBO ERROR AL EJECUTAR AL BUSCAR UN FRAME LIBRE");
		}
		proceso_->contador++;
		if(proceso_->contador >= MARCOS_POR_PROCESO){
			proceso_->puntero=proceso_->contador%(list_size(proceso_->pagMem)+1);
		}else{
			proceso_->puntero = proceso_->contador;
		}
		entrada->frame= frame_libre;
		log_trace(logger, "FRAME AGREGADO: %d", entrada->frame);
	}
	entrada->bPres=1;
	entrada->bUso=1;
	entrada->bMod=0;
	log_trace(logger, "ENTRADA A AGREGAR O REEMPLAZAR");
	log_trace(logger, "ENTRADA :%d || BIT USO: %d || BIT MOD: %d || BIT PRES: %d || FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);
	pagina->entrada = entrada;
	//int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
	int numero_pagina = entrada_tabla_2*ENTRADAS_POR_TABLA +indice_tabla_2;
	log_trace(logger, "CPU || Nro pagina = %d", numero_pagina);
	log_trace(logger, "Nos vamo para swap");
	traer_a_memoria(pid_,numero_pagina,entrada->frame);
	log_trace(logger, "Hemos Volvido a memoria");
	list_add_sorted(proceso_->pagMem,pagina,ordenar);
	log_trace(logger, "CPU || OPERACION DE PAGINA EXITOSA");
	log_trace(logger, "CPU || PID: %d ||TABLA DE PAGINAS EN MEMORIA: ",pid_);
	mostrar_tabla_pagina();
	mostrar_bitarray();
}
bool ordenar(void* entrada1, void* entrada2){

	t_memory_pag* entrada = entrada1;
	t_memory_pag* aux = entrada2;

	return entrada->entrada->frame < aux->entrada->frame;

}
void ejecutar_algoritmo(t_entrada_2* entrada){
	log_trace(logger, "Tipo de algoritmo");
	if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK")==0){
		log_trace(logger, "CLOCK");
		clock_(entrada);
		}
	else if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK-M")==0){
		log_trace(logger, "CLOCK-M");
//		clock_M(proceso_,entrada);
		clock_M(entrada);
	} else{
		log_error(logger,"ERROR EN LEER EL ALGORITMO");
		return;
	}
	log_trace(logger,"TABLA DE PAGINAS DESPUES DE EJECUTAR EL ALGORITMO: ");
	mostrar_tabla_pagina();
}

void clock_(t_entrada_2* entrada){
	log_trace(logger, "CPU || VAMOS A EJECUTAR CLOCK");
	log_trace(logger,"CPU || PUNTERO INICIAL: %d || CONTADOR: %d", proceso_->puntero, proceso_->contador);
	log_trace(logger,"CPU || SE VA A EJECUTAR EL ALGORITMO SOBRE LA SIGUIENTE TABLA: ");
	mostrar_tabla_pagina();
	while(1){
		log_trace(logger, "CPU || BUSCO VICTIMA");
		t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);
		if(posible_victima->entrada->bUso==0){
			log_trace(logger, "CPU || ENCONTRE LA VICTIMA");
			t_memory_pag* victima= posible_victima;
			// ANTES DE REEMPLAZAR LA ENTRADA
			victima->entrada->bPres=0;
			entrada->frame= posible_victima->entrada->frame;
			if(victima->entrada->bMod==1){
				log_trace(logger, "CPU || El maquina tiene el bit de modificado en 1");
				t_swap* swap= malloc(sizeof(t_swap));
				log_trace(logger, "CPU || CREO LA ESTRUCTURA SWAP, CON PID: %d",pid_);
				swap->pid=pid_;
				swap->memorias_a_swappear= list_create();
				log_trace(logger, "CPU || Agrego la victima a la lista de paginas a swapear");
				list_add(swap->memorias_a_swappear,victima);
				log_trace(logger, "CPU || AGREGUE LA VICTIMA A LA LISTA SWAP");
				log_trace(logger, "CPU || PEDIDO AGREGADO");
				list_add(pedidos_swap_l,swap);
				sem_post(&s_swap);
				log_trace(logger, "PASE NOMAAAS");
			}

			victima->entrada = entrada;
			proceso_->contador++;
			log_trace(logger, "CPU || CONTADOR: %d", proceso_->contador);
			proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem));
			log_trace(logger, "CPU || PUNTERO: %d", proceso_->puntero);
			entrada->frame= posible_victima->entrada->frame;
//			return list_remove(proceso_->pagMem,proceso_->puntero);
			return;
		} else{
			posible_victima->entrada->bUso=0;
		}
		proceso_->contador++;
		log_trace(logger, "CPU || CONTADOR: %d", proceso_->contador);
		proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
		log_trace(logger, "CPU || PUNTERO: %d", proceso_->puntero);
	}
}
void clock_M(t_entrada_2* entrada){
	log_trace(logger, "CPU || VAMOS A EJECUTAR CLOCK-M");
	log_trace(logger,"CPU || PUNTERO INICIAL: %d || CONTADOR: %d", proceso_->puntero, proceso_->contador);
	log_trace(logger,"CPU || SE VA A EJECUTAR EL ALGORITMO SOBRE LA SIGUIENTE TABLA: ");
	mostrar_tabla_pagina();
	while(1){
		log_trace(logger, "Obtengo una posible victima");
		for(int i = 0; i < list_size(proceso_->pagMem); i++){
			t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);

			log_trace(logger, "Veo si tiene el buso y bmod en 0");

			if(posible_victima->entrada->bUso==0 && posible_victima->entrada->bMod==0){
				t_memory_pag* victima= posible_victima;
				// ANTES DE REEMPLAZAR LA ENTRADA
				victima->entrada->bPres=0;
				entrada->frame= posible_victima->entrada->frame;
				log_trace(logger, "Tiene el b de usoy b de mod en 0, se hizo la vistima");
				victima->entrada = entrada;
				proceso_->contador++;
				log_trace(logger, "Contador: %d", proceso_->contador);
				proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem));
				log_trace(logger, "Puntero: %d", proceso_->puntero);
				entrada->frame= posible_victima->entrada->frame;
				log_trace(logger, "Se asigno el frame %d", entrada->frame);
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
				log_trace(logger, "Tiene el b de usoy b de mod en 1, se hizo la vistima");
				if(victima->entrada->bMod==1){
					log_trace(logger, "CPU || El maquina tiene el bit de modificado en 1");
					t_swap* swap= malloc(sizeof(t_swap));
					log_trace(logger, "CPU || CREO LA ESTRUCTURA SWAP, CON PID: %d",pid_);
					swap->pid=pid_;
					swap->memorias_a_swappear= list_create();
					log_trace(logger, "CPU || Agrego la victima a la lista de paginas a swapear");
					list_add(swap->memorias_a_swappear,victima);
					log_trace(logger, "CPU || AGREGUE LA VICTIMA A LA LISTA SWAP");
					log_trace(logger, "CPU || PEDIDO AGREGADO");
					list_add(pedidos_swap_l,swap);
					sem_post(&s_swap);
					log_trace(logger, "PASE NOMAAAS");
				}
				victima->entrada = entrada;
				proceso_->contador++;
				proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem));
				entrada->frame= posible_victima->entrada->frame;
				return;
			}
			else{
				log_trace(logger, "Pongo el b de uso en 0 ");
				posible_victima->entrada->bUso=0;
			}
			proceso_->contador++;
			proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
		}
	}
}

void escritura(){
	uint32_t direccion_fisica;
	recv(socket_cpu,&direccion_fisica,sizeof(uint32_t),0);
	uint32_t escritura;
	recv(socket_cpu,&escritura,sizeof(uint32_t),0);
	memcpy(memoria+direccion_fisica,&escritura,sizeof(uint32_t));
	send(socket_cpu,&escritura,sizeof(uint32_t),0);
	t_entrada_2* entrada=  obtener_entrada();
	entrada->bUso=1;
	entrada->bMod=1;
	log_trace(logger, "SE REALIZO LA ESCRITURA %d || EN EL FRAME: %d",escritura, entrada->frame );
}


void lectura(){
	uint32_t direccion_fisica;
	recv(socket_cpu,&direccion_fisica,sizeof(uint32_t),0);
	uint32_t buffer;
	memcpy(&buffer,memoria+direccion_fisica,sizeof(uint32_t));
	log_trace(logger,"VALOR LEIDO: %d", buffer);
	send(socket_cpu,&buffer,sizeof(uint32_t),0);
	t_entrada_2* entrada=  obtener_entrada();
	entrada->bUso=1;
	log_trace(logger, "SE REALIZO LA LECTURA");
}

void mostrar_tabla_pagina(){
	for(int i=0; i < list_size(proceso_->pagMem); i++){
		t_memory_pag* pagina = list_get(proceso_->pagMem,i);
		t_entrada_2* entrada2 = pagina->entrada;
		log_trace(logger,"CPU || ENTRADA || U: %d || M: %d || P: %d || FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
	}
}


void mostrar_entrada(t_entrada_2* entrada2){
	log_trace(logger, "CPU || INDICE GLOBAL 1: %d || ENTRADA (INDICE TABLA 2): %d || ENTRADA TABLA 2: %d	", backup_indice_1,backup_indice_tabla_2,backup_entrada_tabla_2);
	log_trace(logger,"CPU || ENTRADA || U: %d || M: %d || P: %d || FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
}

bool memoria_esta_llena(){
	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){
				if(0==(int)bitarray_test_bit(bitMem,i))
					return false;
		}
	return true;
}

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








