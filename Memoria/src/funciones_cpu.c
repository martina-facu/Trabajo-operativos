#include "funciones_cpu.h"


void* funciones_cpu(){
	
	while(1){
	
	uint8_t operacion;
	recv(socket_cpu,&operacion,sizeof(uint8_t),0);
	log_trace(logger,"CPU || RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);

	switch(operacion){

		case SOLICITAR_VALOR_ENTRADA1:
			log_info(logger, "Lvalor entrada 1");
			entrada1();
			log_info(logger, "enviado valor entrada 1");
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
	log_trace(logger, "CPU || ENTRA DE TABLA 1 RECIBIDO: %d", entrada_tabla_1);
	backup_entrada_tabla_1 = entrada_tabla_1;

	t_tabla_1* tabla= list_get(tabla_1_l,indice_tabla_1);
	pid_ = tabla->pid;
	log_trace(logger, "CPU || PID DE TABLA: %d", pid_);

	log_trace(logger, "Obtengo el proceso %d de la lista de procesos", pid_);
	proceso_ = list_get(procesos,pid_);

	uint32_t indice_entrada_2 = *(tabla->entradas+entrada_tabla_1);
	log_trace(logger, "Obtuve el indice de la entrada 2 -> %d", backup_entrada_tabla_1);

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


	uint32_t indice_tabla_2;
	recv(socket_cpu, &indice_tabla_2, sizeof(uint32_t),0);
	log_trace(logger, "Se recibe el indice de tabla 2 -> %d", indice_tabla_2);

	uint32_t entrada_tabla_2;
	recv(socket_cpu,&entrada_tabla_2,sizeof(uint32_t),0);
	log_trace(logger, "Se recibe la entrada de lat abla 2 -> %d", entrada_tabla_2);
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
	log_trace(logger, "Creo una pagina");
	t_memory_pag *pagina= malloc(sizeof(t_memory_pag));
	pagina->n_tabla_2 = indice_tabla_2;
	pagina->n_entrada_2 = entrada_tabla_2;

	if(list_size(proceso_->pagMem)==MARCOS_POR_PROCESO || memoria_esta_llena()){
		chequear_puntero();
		log_trace(logger, "Todos los marcos estan ocupados, ejecuto algoritmo");
		ejecutar_algoritmo(entrada);
	}else{
		log_trace(logger, "Hay frames libres, asigno uno.");
		int frame_libre = buscar_frame_libre();
		if(frame_libre<0){
			log_trace(logger,"HUBO ERROR AL EJECUTAR AL BUSCAR UN FRAME LIBRE");
		}
		log_trace(logger, "Aumento el contador");
		proceso_->contador++;
		// TODO SI LA MEMORIA ESTA LLENA REINICIAR EL PUNTERO
		if(proceso_->contador >= MARCOS_POR_PROCESO){
			proceso_->puntero=proceso_->contador%(list_size(proceso_->pagMem)+1);
			log_trace(logger, "LE TIRAMO UN RESTO PA , puntero: %d", proceso_->puntero);
		}else{
			proceso_->puntero = proceso_->contador;
			log_trace(logger, "Le mandamo al puntero un contador perro, puntero: %d", proceso_->puntero);
		}
		entrada->frame= frame_libre;
		log_trace(logger, "Se asgina el frame %d", entrada->frame);
	}
	entrada->bPres=1;
	entrada->bUso=1;
	log_trace(logger, "ENTRADA :%d || BIT USO: %d || BIT MOD: %d || BIT PRES: %d || FRAME: %d", entrada_tabla_2,entrada->bUso, entrada->bMod, entrada->bPres, entrada->frame);
	pagina->entrada = entrada;
	log_trace(logger, "Hacemos la cuenta del nro de pagina");
	//int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
	int numero_pagina = entrada_tabla_2*ENTRADAS_POR_TABLA +indice_tabla_2;
	log_trace(logger, "Nro pagina = %d", numero_pagina);
	log_trace(logger, "Nos vamo para swap");
	traer_a_memoria(pid_,numero_pagina,entrada->frame);
	log_trace(logger, "Hemos Volvido a memoria");
	list_add(proceso_->pagMem,pagina);
	log_trace(logger, "Se agrego una pagina a la lista de paginas en memoria");
	mostrar_bitarray();
}
bool ordenar(void* entrada1, void* entrada2){

	t_entrada_2* entrada = entrada1;
	t_entrada_2* aux = entrada2;

	return entrada->frame < aux->frame;

}
void ejecutar_algoritmo(t_entrada_2* entrada){
	t_memory_pag* victima;
	log_trace(logger, "Tipo de algoritmo");
	if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK")==0){
		log_trace(logger, "CLOCKKKKKK");
		victima = clock_();
		if(victima->entrada->bMod==1){
			log_trace(logger, "CPU || El maquina tiene el bit de modificado en 1");
			t_swap* swap= malloc(sizeof(t_swap));
			log_trace(logger, "CPU || CREO LA ESTRUCTURA SWAP, CON PID: %d",pid_);
			swap->pid=pid_;;
			swap->memorias_a_swappear= list_create();
			log_trace(logger, "CPU || Agrego la victima a la lista de paginas a swapear");
			list_add(swap->memorias_a_swappear,victima);
			log_trace(logger, "CPU || AGREGUE LA VICTIMA A LA LISTA SWAP");
			log_trace(logger, "CPU || PEDIDO AGREGADO");
			list_add(pedidos_swap_l,swap);
			sem_post(&s_swap);
			log_trace(logger, "PASE NOMAAAS");
		}

	} else if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK-M")==0){
//		clock_M(proceso_,entrada);
		victima = clock_M();
		if(victima->entrada->bMod==1){
			log_trace(logger, "CPU || El maquina tiene el bit de modificado en 1");
			t_swap* swap= malloc(sizeof(t_swap));
			log_trace(logger, "CPU || CREO LA ESTRUCTURA SWAP, CON PID: %d",pid_);
			swap->pid=pid_;;
			swap->memorias_a_swappear= list_create();
			log_trace(logger, "CPU || Agrego la victima a la lista de paginas a swapear");
			list_add(swap->memorias_a_swappear,victima);
			log_trace(logger, "CPU || AGREGUE LA VICTIMA A LA LISTA SWAP");
			log_trace(logger, "CPU || PEDIDO AGREGADO");
			list_add(pedidos_swap_l,swap);
			sem_post(&s_swap);
			log_trace(logger, "PASE NOMAAAS");
		}
	} else{
		log_error(logger,"ERROR EN LEER EL ALGORITMO");
		return;
	}
	log_trace(logger, "Voy a modificar datos de la victima");
	victima->entrada->bPres=0;
	proceso_->contador++;
	log_trace(logger, "Contador: %d", proceso_->contador);
	proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem)+1);
	log_trace(logger, "Puntero: %d", proceso_->puntero);
	entrada->frame= victima->entrada->frame;
	log_trace(logger, "Se asigno el frame %d", entrada->frame);
}

t_memory_pag* clock_(){
	log_trace(logger, "CPU || VAMOS A EJECUTAR CLOCK");
	log_trace(logger,"CPU || PUNTERO INICIAL: %d || CONTADOR: %d", proceso_->puntero, proceso_->contador);
	mostrar_tabla_pagina();
	while(1){
		log_trace(logger, "CPU || BUSCO VICTIMA");
		t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);
		if(posible_victima->entrada->bUso==0){
			log_trace(logger, "CPU || ENCONTRE LA VICTIMA");
			return list_remove(proceso_->pagMem,proceso_->puntero);

		} else{
			log_trace(logger, "CPU || PONGO BIT DE USO EN 0 ");
			posible_victima->entrada->bUso=0;
		}
		proceso_->contador++;
		log_trace(logger, "CPU || CONTADOR: %d", proceso_->contador);
		proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
		log_trace(logger, "CPU || PUNTERO: %d", proceso_->puntero);
	}
}
t_memory_pag* clock_M(){
	log_trace(logger, "CPU || VAMOS A EJECUTAR CLOCK-M");
	log_trace(logger,"CPU || PUNTERO INICIAL: %d || CONTADOR: %d", proceso_->puntero, proceso_->contador);
	while(1){
		mostrar_tabla_pagina();
		log_trace(logger, "Obtengo una posible victima");
		for(int i = 0; i < list_size(proceso_->pagMem); i++){
			t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);

			log_trace(logger, "Veo si tiene el buso y bmod en 0");

			if(posible_victima->entrada->bUso==0 && posible_victima->entrada->bMod==0){

				log_trace(logger, "Tiene el b de usoy b de mod en 0, se hizo la vistima");
				return list_remove(proceso_->pagMem,proceso_->puntero);
			}

			log_trace(logger, "Contador %d", proceso_->contador);
			proceso_->contador++;
			log_trace(logger, "Contador %d", proceso_->contador);
			proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
			log_trace(logger, "Puntero %d", proceso_->puntero);
		}

		for(int i = 0; i < list_size(proceso_->pagMem); i++){
			t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);

			if (posible_victima->entrada->bUso==0 && posible_victima->entrada->bMod==1){

				log_trace(logger, "Tiene el b de usoy b de mod en 1, se hizo la vistima");
				return list_remove(proceso_->pagMem,proceso_->puntero);
			}
			else{
				log_trace(logger, "Pongo el b de uso en 0 ");
				posible_victima->entrada->bUso=0;
			}

			log_trace(logger, "Contador %d", proceso_->contador);
			proceso_->contador++;
			log_trace(logger, "Contador %d", proceso_->contador);
			proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
			log_trace(logger, "Puntero %d", proceso_->puntero);
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
	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){
					if(!bitarray_test_bit(bitMem,i)){
						bitarray_set_bit(bitMem,i);
						return i;
					}
		}
	return -1;
}








