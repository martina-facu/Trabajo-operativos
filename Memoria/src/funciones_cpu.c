#include "funciones_cpu.h"


void* funciones_cpu(){
	
	while(1){
	
	uint8_t operacion;
	recv(socket_cpu,&pid_,sizeof(uint32_t),0);
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
	uint32_t entrada_tabla_1;
	recv(socket_cpu,&entrada_tabla_1,sizeof(uint32_t),0);
	proceso_ = list_get(procesos,pid_);
	t_tabla_1* tabla= list_get(tabla_1_l,proceso_->entrada_1);
	uint32_t indice_entrada_2 = *(tabla->entradas+entrada_tabla_1);
	entrada_backup_tabla_1 = indice_entrada_2;
	send(socket_cpu,&indice_entrada_2,sizeof(uint32_t),0);
}

void entrada2(){
	uint32_t entrada_tabla_2;
	recv(socket_cpu,&entrada_tabla_2,sizeof(uint32_t),0);
	t_tabla_2* tabla2= list_get(tabla_2_l,entrada_backup_tabla_1);
	t_entrada_2* entrada = list_get(tabla2->entradas,entrada_tabla_2);
	if(entrada->bPres==0){
		page_fault(entrada,entrada_backup_tabla_1,entrada_tabla_2);
	}
	send(socket_cpu,&entrada->frame,sizeof(uint32_t),0);
}

void page_fault(t_entrada_2* entrada,uint32_t entrada_backup_tabla_1, uint32_t entrada_tabla_2){
	t_memory_pag *pagina= malloc(sizeof(t_memory_pag));
	pagina->n_tabla_2 = entrada_backup_tabla_1;
	pagina->n_entrada_2 = entrada_tabla_2;
	if(list_size(proceso_->pagMem)==MARCOS_POR_PROCESO || memoria_esta_llena()){
		ejecutar_algoritmo(entrada);
	}else{
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
	}
	entrada->bPres=1;
	entrada->bUso=1;
	pagina->entrada = entrada;
	int numero_pagina = entrada_backup_tabla_1*ENTRADAS_POR_TABLA +entrada_tabla_2;
	traer_a_memoria(pid_,numero_pagina,entrada->frame);
	list_add(proceso_->pagMem,pagina);
}
bool ordenar(void* entrada1, void* entrada2){

	t_entrada_2* entrada = entrada1;
	t_entrada_2* aux = entrada2;

	return entrada->frame < aux->frame;

}
void ejecutar_algoritmo(t_entrada_2* entrada){
	t_memory_pag* victima;
	entrada->bPres=1;
	if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK")==0){
		victima = clock_();
		if(victima->entrada->bMod==1){
			t_swap* swap= malloc(sizeof(t_swap));
			swap->pid=pid_;
			swap->memorias_a_swappear= list_create();
			list_add(swap->memorias_a_swappear,victima);

			sem_post(&s_swap);
		}

	} else if(strcmp(ALGORITMO_REEMPLAZO, "CLOCK-M")){
//		clock_M(proceso_,entrada);
		victima = clock_(proceso_);
	} else{
		log_error(logger,"ERROR EN LEER EL ALGORITMO");
		return;
	}
	victima->entrada->bPres=0;
	proceso_->contador++;
	proceso_->puntero= proceso_->contador%(list_size(proceso_->pagMem)+1);
	entrada->frame= victima->entrada->frame;
}

t_memory_pag* clock_(){
	while(1){
		t_memory_pag* posible_victima = list_get(proceso_->pagMem,proceso_->puntero);
		if(posible_victima->entrada->bUso==0){
			return list_remove(proceso_->pagMem,proceso_->puntero);

		} else{
			posible_victima->entrada->bUso=0;
		}
		proceso_->contador++;
		proceso_->puntero= proceso_->contador%list_size(proceso_->pagMem);
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







































