//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>
//#include <ctype.h>
//#include <stdlib.h>
//#include <commons/collections/list.h>
//#include <pthread.h>
//#include <semaphore.h>
//#include <unistd.h>
//#include <time.h>
//
//clock_t tiempo_de_ejecucion_inicial;
//clock_t tiempo_de_ejecucion_final;
//float tiempo_de_ejecucion;
//
//float alpha;
//
//typedef enum {BLOQUEADO, FINALIZADO,SUSPENDIDO,INTERRUMPIDO} ESTADO;
//
//const int grado_multiprogramacion;
//
//int TIEMPO_BLOCK_MAX=0;
//
//t_list* ready_l;
//t_list* new_l;
//t_list* susp_ready_l;
//t_list* susp_ready_buffer_l;
//t_list* block_l;
//t_list* unblock_l;
//t_list* finalizado_l;
//t_list* block_susp_l;
//t_list* exit_l;
//t_list* interrumpidos_l;
//
//pthread_mutex_t mx_ready_l;
//pthread_mutex_t mx_new_l;
//pthread_mutex_t mx_susp_ready_l;
//pthread_mutex_t mx_block_l;
//pthread_mutex_t mx_unblock_l;
//pthread_mutex_t mx_finalizado_l;
//pthread_mutex_t mx_block_susp_l;
//pthread_mutex_t mx_susp_ready_buffer_l;
//pthread_mutex_t mx_interrumpidos_l;
//
//sem_t s_proceso_ready;
//sem_t s_proceso_susp;
//sem_t s_cpu;
//sem_t s_proceso_finalizado;
//sem_t s_proceso_new;
//sem_t s_grado_multiprogramacion;
//sem_t s_proceso_susp_ready;
//sem_t s_interrupcion;
//
//
//typedef struct{
//	uint8_t id;
//	uint32_t tiempo_block;
//	ESTADO estado;
//	float estimacion;
//}pcb_t;
//
//void mostrar_lista_ready(t_list* lista){
//	int i;
//	int lista_size= list_size(lista);
//	for(i=0;i<lista_size;i++){
//		pcb_t* pcb= malloc(sizeof(pcb_t));
//		pcb= list_get(lista,i);
//		printf("PCB = %d, prioridad= %d \n", pcb->id,i);
//	}
//}
//
////bool proceso_desbloqueado(void* pcb){
////	pcb_t* pcb_= pcb;
////	return (pcb_->condicion==UNBLOCK);
////}
//
//void* agregar_a_ready(){ // REVISAR(no estoy seguro que sea un problema): SI SE BLOQUEO EL HILO EN NEW Y, MIENTRAS ESTA BLOQUEADO, SE AGREGA UN NUEVO PROCESO EN SUSP READY, VA A SACAR EL DE NEW PRIMERO
//	while(1){
//		sem_wait(&s_proceso_ready);
//		pcb_t* pcb;
//		if(!list_is_empty(unblock_l)){
//			pthread_mutex_lock(&mx_unblock_l);
//			pcb= list_remove(unblock_l,0);
//			pthread_mutex_unlock(&mx_unblock_l);
//		}
//		if(!list_is_empty(susp_ready_l)){
//			pthread_mutex_lock(&mx_susp_ready_l);
//			pcb= list_remove(susp_ready_l,0);
//			pthread_mutex_unlock(&mx_susp_ready_l);
//		}
//		if(!list_is_empty(new_l)){
//			pthread_mutex_lock(&mx_new_l);
//			pcb= list_remove(new_l,0);
//			pthread_mutex_unlock(&mx_new_l);
//		}
//		pthread_mutex_lock(&mx_ready_l);
//		list_add(ready_l,pcb);
//		pthread_mutex_unlock(&mx_ready_l);
//		mostrar_lista_ready(ready_l);
//	}
//	return NULL;
//}
//
//
//void* enviar_a_ejecutar()  {
//	while(1){
//		sem_wait(&s_cpu);
//		pthread_mutex_lock(&mx_ready_l);
//		pcb_t* pcb= list_remove(ready_l,0);
//		pthread_mutex_unlock(&mx_ready_l);
//		// TODO ENVIAR AL CPU
//	}
//	return NULL;
//}
//
//void remover_de_lista(t_list* lista,pcb_t* pcb){
//	for(int i=0;i<list_size(lista);i++){
//		pcb_t* pcb_aux= list_get(lista,i);
//		if(pcb_aux->id == pcb->id){
//			list_remove(lista,i);
//			return;
//		}
//	}
//}
//
//void* bloquear_proceso(void* pcb_){
//	pcb_t* pcb= pcb_;
//	if(pcb->tiempo_block>TIEMPO_BLOCK_MAX){
//		pthread_mutex_lock(&mx_block_l);
//		remover_de_lista(block_l,pcb);
//		pthread_mutex_unlock(&mx_block_l);
//
//		pthread_mutex_lock(&mx_susp_ready_buffer_l);
//		list_add(susp_ready_buffer_l,pcb);
//		pthread_mutex_unlock(&mx_susp_ready_buffer_l);
//
//		sem_post(&s_proceso_susp);
//
//		pcb->estado= SUSPENDIDO;
//
//		usleep(pcb->tiempo_block);
//
//		pthread_mutex_lock(&mx_block_susp_l);
//		remover_de_lista(block_susp_l,pcb);
//		pthread_mutex_unlock(&mx_block_susp_l);
//
//		pthread_mutex_lock(&mx_susp_ready_l);
//		list_add(susp_ready_l,pcb);
//		pthread_mutex_unlock(&mx_susp_ready_l);
//		// TODO avisar a la memoria que se libere
//		sem_post(&s_proceso_susp_ready);
//	}
//	else{
//		usleep(pcb->tiempo_block);
//
//		pthread_mutex_lock(&mx_block_l);
//		remover_de_lista(block_l,pcb);
//		pthread_mutex_unlock(&mx_block_l);
//
//		pthread_mutex_lock(&mx_unblock_l);
//		list_add(unblock_l,pcb);
//		pthread_mutex_unlock(&mx_unblock_l);
//
//	}
//	sem_post(&s_proceso_ready);
//	return NULL;
//}
//
//
//void* recibir_proceso_de_cpu(){
//	pcb_t* pcb;
//	if(pcb->estado == FINALIZADO){ // SE PODRIA MODELAR CON UN SWITCH
//		pthread_mutex_lock(&mx_finalizado_l);
//		list_add(finalizado_l,pcb);
//		pthread_mutex_unlock(&mx_finalizado_l);
//
//		sem_post(&s_proceso_finalizado);
//
//		return NULL;
//	}
//	else if(pcb->estado==BLOQUEADO){
//		pthread_mutex_lock(&mx_block_l);
//		list_add(block_l,pcb);
//		pthread_mutex_unlock(&mx_block_l);
//
//		pthread_t hilo_bloqueante;
//		pthread_create(&hilo_bloqueante,NULL,bloquear_proceso,pcb);
//		pthread_detach(hilo_bloqueante);
//
//		return NULL;
//	}
//	else{
//		printf("ocurrio un error al recibir al proceso: funcion: recibir_proceso_de_cpu \n");
//		return NULL;
//	}
//}
//
//void* fifo(){
//	pthread_t hilo1;
////	pthread_t hilo2; DESCOMENTAR CUANDO SE IMPLEMENTEN LAS CONEXIONES
////	pthread_t hilo3; DESCOMENTAR CUANDO SE IMPLEMENTEN LAS CONEXIONES
//
//	pthread_create(&hilo1,NULL,agregar_a_ready,NULL);
////	pthread_create(&hilo2,NULL,enviar_a_ejecutar,NULL); DESCOMENTAR CUANDO SE IMPLEMENTEN LAS CONEXIONES
////	pthread_create(&hilo3,NULL,recibir_proceso_de_cpu,NULL); DESCOMENTAR CUANDO SE IMPLEMENTEN LAS CONEXIONES
//
//	pthread_detach(hilo1);
////	pthread_detach(hilo2); DESCOMENTAR CUANDO SE IMPLEMENTEN LAS CONEXIONES
////	pthread_join(hilo3,NULL); DESCOMENTAR CUANDO SE IMPLEMENTEN LAS CONEXIONES
//	return NULL;
//}
////--------------------------------------------------------------------PLANIFICADOR LARGO PLAZO----------------------------------------------------------------------//
////pcb_t* crear_pcb(t_list* lista){
////
////	// TODO
////
////	pcb_t* pcb= NULL;
////	return pcb;
////
////}
////void* administrador_mediano_plazo(){
////	while(1){
////		sem_wait(&s_proceso_susp_ready);
////		sem_post(&s_proceso_ready);
////	}
////}
//
//void* agregar_estado_new(){
//	//TODO  comunicacion con las consola y crear el pcb, definiendo la estimacion inicial (MUCHO MUY IMPORTANTE)
//	pcb_t* pcb;
//	int aux;
//	while(1){
//		pcb= malloc(sizeof(pcb_t));
//		printf("Decime un PCB\n");
//		scanf("%d",&aux);
//		pcb->id= aux;
//		fflush(stdin);
//		list_add(new_l,pcb);
//		sem_post(&s_proceso_new);
//	}
//	return NULL;
//}
//
//void* pasar_a_ready(){
//	while(1){
//		sem_wait(&s_proceso_new);
//		sem_wait(&s_grado_multiprogramacion);
//		sem_post(&s_proceso_ready);
//	}
//	return NULL;
//}
//
//void* finalizar_procesos(){
//	while(1){
//		sem_wait(&s_proceso_finalizado);
//		pcb_t* pcb_finalizado;
//
//		pthread_mutex_lock(&mx_finalizado_l);
//		pcb_finalizado=list_remove(finalizado_l,0);
//		pthread_mutex_unlock(&mx_finalizado_l);
//
//		list_add(exit_l,pcb_finalizado);
//
//		// TODO avisar a memoria que libere la memoria
//
//		// TODO avisar a consola que finalice
//	}
//	return NULL;
//}
//
//void* administrador_largo_plazo(){
//
//	pthread_t hilo1;
//	pthread_t hilo2;
//	pthread_t hilo3;
//
//
//	pthread_create(&hilo2,NULL,pasar_a_ready,NULL);
//	pthread_detach(hilo2);
//
//	pthread_create(&hilo3,NULL,finalizar_procesos,NULL);
//	pthread_detach(hilo3);
//
//	pthread_create(&hilo1,NULL,agregar_estado_new,NULL);
//	pthread_join(hilo1,NULL);
//
//	return NULL;
//}
////--------------------------------------------------------------------   SJF   ---------------------------------------------------------------------//
////void* agregar_a_ready_sjf(){
////
////
////}
////
//
//void* enviar_a_ejecutar_sjf(){
//	while(1){
//		sem_wait(&s_cpu);
//		pthread_mutex_lock(&mx_ready_l);
//		pcb_t* pcb= list_remove(ready_l,0);
//		pthread_mutex_unlock(&mx_ready_l);
//		//TODO ENVIAR A CPU
//		tiempo_de_ejecucion_inicial = clock();
//	}
//	return NULL;
//}
//
//
//void* interrupciones(){
//	while(1){
//		sem_wait(&s_interrupcion);
//		//avisar al cpu que me devuelva el proceso;
//		pcb_t* pcb=malloc(sizeof(pcb_t));
//		list_add(interrumpidos_l,pcb);
//	}
//}
//
//void* actualizar_estimacion(pcb_t* pcb){
//	pcb->estimacion = pcb->estimacion*alpha+(1-alpha)*tiempo_de_ejecucion;
//	return NULL;
//}
//
//
//void* devoluciones(){
//	while(1){
//		// TODO recv del cpu
//		pcb_t* pcb=malloc(sizeof(pcb_t));
//		tiempo_de_ejecucion_final=clock();
//		tiempo_de_ejecucion= (tiempo_de_ejecucion_final - tiempo_de_ejecucion_inicial)/(CLOCKS_PER_SEC*1000); // divido por mil para dejarlo en milisegundos
//		actualizar_estimacion(pcb);
//		switch(pcb->estado){
//		case INTERRUMPIDO: {
//			//TODO
//			break;
//		}
//		case BLOQUEADO:{
//			pthread_t hilo_bloqueante;
//			pthread_create(&hilo_bloqueante,NULL,bloquear_proceso,pcb);
//			pthread_detach(hilo_bloqueante);
//			break;
//		}
//		case FINALIZADO:{
//			pthread_mutex_lock(&mx_finalizado_l);
//			list_add(finalizado_l,pcb);
//			pthread_mutex_unlock(&mx_finalizado_l);
//
//			sem_post(&s_proceso_finalizado);
//			break;
//		}
//		default:{
//			printf("error en la devolucion");
//			break;
//		}
//	}
//	}
//	return NULL;
//}
//
//void* hablar_con_cpu(){
//	pthread_t hilo1;
//	pthread_t hilo2;
//	pthread_t hilo3;
//	pthread_create(&hilo1,NULL,interrupciones,NULL);
//	pthread_create(&hilo3,NULL,enviar_a_ejecutar_sjf,NULL);
//	pthread_create(&hilo2,NULL,devoluciones,NULL);
//	pthread_detach(hilo1);
//	pthread_detach(hilo2);
//	pthread_detach(hilo3);
//	return NULL;
//}
//
//bool menor_estimacion(void* pcb_aux,void* pcb_aux2){
//	pcb_t* pcb1 =pcb_aux;
//	pcb_t* pcb2 = pcb_aux2;
//	return pcb1->estimacion<pcb2->estimacion;
//}
//
//
//void* agregar_a_ready_sjf(){
//	while(1){
//		sem_wait(&s_proceso_ready);
//		pcb_t* pcb;
//		if(!list_is_empty(interrumpidos_l)){
//			pthread_mutex_lock(&mx_interrumpidos_l);
//			pcb= list_remove(interrumpidos_l,0);
//			pthread_mutex_unlock(&mx_interrumpidos_l);
//		}
//		if(!list_is_empty(unblock_l)){
//			pthread_mutex_lock(&mx_unblock_l);
//			pcb= list_remove(unblock_l,0);
//			pthread_mutex_unlock(&mx_unblock_l);
//		}
//		if(!list_is_empty(susp_ready_l)){
//			pthread_mutex_lock(&mx_susp_ready_l);
//			pcb= list_remove(susp_ready_l,0);
//			pthread_mutex_unlock(&mx_susp_ready_l);
//		}
//		if(!list_is_empty(new_l)){
//			pthread_mutex_lock(&mx_new_l);
//			pcb= list_remove(new_l,0);
//			pthread_mutex_unlock(&mx_new_l);
//		}
//		pthread_mutex_lock(&mx_ready_l);
//		list_add_sorted(ready_l, pcb, menor_estimacion);
//		pthread_mutex_unlock(&mx_ready_l);
//		mostrar_lista_ready(ready_l);
//
//	}
//	return NULL;
//}
//
//void* sjf(){
//	pthread_t hilo1;
//	pthread_t hilo2;
//
//	pthread_create(&hilo1,NULL,agregar_a_ready_sjf,NULL);
//	pthread_create(&hilo2,NULL,hablar_con_cpu,NULL);
//
//	pthread_detach(hilo1);
//	pthread_detach(hilo2);
//	return NULL;
//
//}
//
////--------------------------------------------------------------------PLANIFICADOR MEDIANO PLAZO--------------------------------------------------------------------//
//void* suspender_proceso(){
//	while(1){
//		sem_wait(&s_proceso_susp);
//		pthread_mutex_lock(&mx_susp_ready_buffer_l);
//		pcb_t* pcb= list_remove(susp_ready_buffer_l,0);
//		pthread_mutex_unlock(&mx_susp_ready_buffer_l);
//
//		pthread_mutex_lock(&mx_susp_ready_l);
//		list_add(susp_ready_l,pcb);
//		pthread_mutex_unlock(&mx_susp_ready_l);
//		sem_post(&s_grado_multiprogramacion);
//
//	}
//	return NULL;
//}
//
//void* avisar_proceso_susp_ready(){
//	while(1){
//		sem_wait(&s_proceso_susp_ready);
//		sem_post(&s_proceso_ready);
//		}
//	return NULL;
//}
//
//
//
//void* administrador_mediano_plazo(){
//	pthread_t hilo1;
//	pthread_t hilo2;
//
//
//	pthread_create(&hilo1,NULL,suspender_proceso,NULL);
//	pthread_detach(hilo1);
//
//	pthread_create(&hilo2,NULL,avisar_proceso_susp_ready,NULL);
//	pthread_detach(hilo2);
//
//
//	return NULL;
//}
//
//
//int main(void){
//	pthread_t planificador_corto_plazo;
//	pthread_t planificador_mediano_plazo;
//	pthread_t planificador_largo_plazo;
//	int grado_multiprogramacion=5;
//
//	sem_init(&s_proceso_ready,0,0);
//	sem_init(&s_cpu,0,0);
//	sem_init(&s_proceso_finalizado,0,0);
//	sem_init(&s_proceso_new,0,0);
//	sem_init(&s_grado_multiprogramacion,0,grado_multiprogramacion);// contador
//	sem_init(&s_proceso_susp,0,0);
//	sem_init(&s_interrupcion,0,0);
//
//	ready_l= 				list_create();
//	new_l= 					list_create();
//	susp_ready_l=			list_create();
//	block_l=				list_create();
//	unblock_l= 				list_create();
//	block_susp_l= 			list_create();
//	exit_l=					list_create();
//	susp_ready_buffer_l=    list_create();
//
//	pthread_create(&planificador_corto_plazo,NULL,fifo,NULL);
//	pthread_join(planificador_corto_plazo,NULL);
//
//	pthread_create(&planificador_largo_plazo,NULL,administrador_largo_plazo,NULL);
//	pthread_join(planificador_largo_plazo,NULL);
//	pthread_create(&planificador_mediano_plazo,NULL,administrador_mediano_plazo,NULL);
//	pthread_join(planificador_mediano_plazo,NULL);
//
//	return EXIT_SUCCESS;
//}
