#include "funciones_cpu.h"


void* funciones_cpu(){
	
	while(1){
	
	uint8_t operacion;
	recv(socket_cpu,&operacion,sizeof(uint8_t),0);
	log_trace(logger,"RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);

	switch(operacion){

		case SOLICITAR_VALOR_ENTRADA1:
			entrada1();
		break;
		case SOLICITAR_VALOR_ENTRADA2:
			entrada2();
		break;
		case SOLICITAR_LECTURA:

		break;
		case SOLICITAR_ESCRITURA:
		break;

		default:
			log_error(logger, "El mensaje recibido %d no corresponde a uno de los conocidos.", operacion);
			log_error(logger, "Se procede a cerrar la conexion");
			close(socket_cpu);
			break;
		}

	}

	return NULL;
}

void entrada1(){

	log_trace(logger, "Vamos a recibir el indice de entrada");
	uint32_t indice_tabla_1;
	recv(socket_cpu, &indice_tabla_1, sizeof(uint32_t),0);

	log_trace(logger, "Vamos a recibir la entrada 1");
	uint32_t entrada_tabla_1;
	recv(socket_cpu,&entrada_tabla_1,sizeof(uint32_t),0);

	t_tabla_1* tabla= list_get(tabla_1_l,indice_tabla_1);
	log_trace(logger, "Obtengo la tabla en el indice %d", entrada_tabla_1);
	pid_ = tabla->pid;

	log_trace(logger, "Obtengo el proceso %d de la lista de procesos", pid_);
	proceso_ = list_get(procesos,pid_);

	uint32_t indice_entrada_2 = *(tabla->entradas+entrada_tabla_1);
	entrada_backup_tabla_1 = indice_entrada_2;
	log_trace(logger, "Obtuve el indice de la entrada 2 -> %d", entrada_backup_tabla_1);

	send(socket_cpu,&indice_entrada_2,sizeof(uint32_t),0);
	log_trace(logger, "Se envio el mensaje a cpu");
}

void entrada2(){


	uint32_t indice_tabla_2;
	recv(socket_cpu, &indice_tabla_2, sizeof(uint32_t),0);
	log_trace(logger, "Se recibe el indice de tabla 2 -> %d", indice_tabla_2);

	uint32_t entrada_tabla_2;
	recv(socket_cpu,&entrada_tabla_2,sizeof(uint32_t),0);
	log_trace(logger, "Se recibe la entrada de lat abla 2 -> %d", entrada_tabla_2);

	uint32_t numero_pagina;
	recv(socket_cpu,&numero_pagina,sizeof(uint32_t),0);
	log_trace(logger, "Se recibe el numero de pagina %d", numero_pagina);

	t_tabla_2* tabla2= list_get(tabla_2_l,indice_tabla_2);
	t_entrada_2* entrada = list_get(tabla2->entradas,entrada_tabla_2);
	log_trace(logger, "Obtenemos la entrada %d de la lista de tablas de nivel 2");

	if(entrada->bPres==0){
		log_trace(logger, "El bit de presencia esta en 0, hacemos page fault");
		page_fault(entrada,indice_tabla_2,entrada_tabla_2);
	}

	log_trace(logger, "Se envia el nro de frame %d", entrada->frame);
	send(socket_cpu,&entrada->frame,sizeof(uint32_t),0);

}

void page_fault(t_entrada_2* entrada,uint32_t indice_tabla_2, uint32_t entrada_tabla_2){
	log_trace(logger, "Creo una pagina");
	t_memory_pag *pagina= malloc(sizeof(t_memory_pag));
	pagina->n_tabla_2 = indice_tabla_2;
	pagina->n_entrada_2 = entrada_tabla_2;

	if(list_size(proceso_->pagMem)==MARCOS_POR_PROCESO || memoria_esta_llena()){
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
		if(proceso_->contador >= MARCOS_POR_PROCESO){
			log_info(logger, "LE TIRAMO UN RESTO PA");
			proceso_->puntero=proceso_->contador%(list_size(proceso_->pagMem)+1);
			log_trace(logger, "%d", proceso_->puntero);
		}else{
			log_trace(logger, "Le mandamo al puntero un contador perro");
			proceso_->puntero = proceso_->contador;
			log_trace(logger, "%d", proceso_->puntero);
		}
		entrada->frame= frame_libre;
		log_trace(logger, "Se asgina el frame %d", entrada->frame);
	}
	entrada->bPres=1;
	entrada->bUso=1;
	pagina->entrada = entrada;
	log_trace(logger, "Hacemos la cuenta del nro de pagina");
	int numero_pagina = indice_tabla_2*ENTRADAS_POR_TABLA +entrada_tabla_2;
	log_trace(logger, "Nro pagina = %d", numero_pagina);
	log_trace(logger, "Nos vamo para swap");
	traer_a_memoria(pid_,numero_pagina,entrada->frame);
	log_trace(logger, "Hemos Volvido a memoria");
	list_add(proceso_->pagMem,pagina);
	log_trace(logger, "Se agrego una pagina a la lista de paginas en memoria");
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
			log_trace(logger, "El maquina tiene el bit de modificado en 1");
			t_swap* swap= malloc(sizeof(t_swap));
			log_trace(logger, "Cree la estructura swap");
			swap->pid=pid_;
			log_trace(logger, "Creo la lista de pag a swapear");
			swap->memorias_a_swappear= list_create();
			log_trace(logger, "Agrego la victima a la lista de paginas a swapear");
			list_add(swap->memorias_a_swappear,victima);

			log_trace(logger, "ALTOOO semaforo");
			sem_post(&s_swap);
			log_trace(logger, "PASE NOMAAAS");
		}

	} else if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK-M")){
//		clock_M(proceso_,entrada);
		victima = clock_(proceso_);
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
	log_trace(logger, "Voy a entrar al while");
	while(1){
		log_trace(logger, "Obtengo una posible victima");
		t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);
		log_trace(logger, "Veo si tiene el buso en 0");
		if(posible_victima->entrada->bUso==0){
			log_trace(logger, "Tiene el b de uso en 0, se hizo la vistima");
			return list_remove(proceso_->pagMem,proceso_->puntero);

		} else{
			log_trace(logger, "Pongo el b de uso en 0 ");
			posible_victima->entrada->bUso=0;
		}
		log_trace(logger, "Contador %d", proceso_->contador);
		proceso_->contador++;
		log_trace(logger, "Contador %d", proceso_->contador);
		proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
		log_trace(logger, "Contador %d", proceso_->puntero);
	}
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
					if(0==(int)bitarray_test_bit(bitMem,i)){
						bitarray_set_bit(bitMem,i);
						return i;
					}
		}
	return -1;
}








