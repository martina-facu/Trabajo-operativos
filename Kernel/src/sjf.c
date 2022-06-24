/*
 * sjf.c
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */


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
//#include "pcb.h"
//#include "listas.h"
//#include "semaforos.h"
//#include "planificador_mediano_plazo.h"
//#include "planificador_largo_plazo.h"
//#include "var_glob.h"
//#include "sjf.h"
//
//void* enviar_a_ejecutar_sjf(){
////	while(1){
////		sem_wait(&s_cpu);
////		pthread_mutex_lock(&mx_ready_l);
////		pcb_t* pcb= list_remove(ready_l,0);
////		pthread_mutex_unlock(&mx_ready_l);
////		//TODO ENVIAR A CPU
////		tiempo_de_ejecucion_inicial = clock();
////	}
////	return NULL;
//}
//
//
//void* interrupciones(){
////	while(1){
////		sem_wait(&s_interrupcion);
////		//avisar al cpu que me devuelva el proceso;
////		pcb_t* pcb=malloc(sizeof(pcb_t));
////		list_add(interrumpidos_l,pcb);
//	}
//}
//
//void* actualizar_estimacion(pcb_t* pcb){
////	pcb->estimado_rafaga = pcb->estimado_rafaga*alpha+(1-alpha)*tiempo_de_ejecucion;
////	return NULL;
//}
//
//void* bloquear_proceso_sjf(void* pcb_){
////	pcb_t* pcb= pcb_;
////	if(pcb->tiempo_block>TIEMPO_BLOCK_MAX){
////		pthread_mutex_lock(&mx_block_l);
////		remover_de_lista(block_l,pcb);
////		pthread_mutex_unlock(&mx_block_l);
////
////		pthread_mutex_lock(&mx_susp_ready_buffer_l);
////		list_add(susp_ready_buffer_l,pcb);
////		pthread_mutex_unlock(&mx_susp_ready_buffer_l);
////
////		sem_post(&s_proceso_susp);
////
////		pcb->estado= SUSPENDIDO;
////
////		usleep(pcb->tiempo_block);
////
////		pthread_mutex_lock(&mx_block_susp_l);
////		remover_de_lista(block_susp_l,pcb);
////		pthread_mutex_unlock(&mx_block_susp_l);
////
////		pthread_mutex_lock(&mx_susp_ready_l);
////		list_add(susp_ready_l,pcb);
////		pthread_mutex_unlock(&mx_susp_ready_l);
////		// TODO avisar a la memoria que se libere
////		sem_post(&s_proceso_susp_ready);
////	}
////	else{
////		usleep(pcb->tiempo_block);
////
////		pthread_mutex_lock(&mx_block_l);
////		remover_de_lista(block_l,pcb);
////		pthread_mutex_unlock(&mx_block_l);
////
////		pthread_mutex_lock(&mx_unblock_l);
////		list_add(unblock_l,pcb);
////		pthread_mutex_unlock(&mx_unblock_l);
////
////	}
////	sem_post(&s_proceso_ready);
////	return NULL;
//}
//
//void* devoluciones(){
////	while(1){
////		// TODO recv del cpu
////		pcb_t* pcb=malloc(sizeof(pcb_t));
////		tiempo_de_ejecucion_final=clock();
////		tiempo_de_ejecucion= (tiempo_de_ejecucion_final - tiempo_de_ejecucion_inicial)/(CLOCKS_PER_SEC*1000); // divido por mil para dejarlo en milisegundos
////		actualizar_estimacion(pcb);
////		switch(pcb->estado){
////		case INTERRUMPIDO: {
////			//TODO
////			break;
////		}
////		case BLOQUEADO:{
////			pthread_t hilo_bloqueante;
////			pthread_create(&hilo_bloqueante,NULL,bloquear_proceso_sjf,pcb);
////			pthread_detach(hilo_bloqueante);
////			break;
////		}
////		case FINALIZADO:{
////			pthread_mutex_lock(&mx_finalizado_l);
////			list_add(finalizado_l,pcb);
////			pthread_mutex_unlock(&mx_finalizado_l);
////
////			sem_post(&s_proceso_finalizado);
////			break;
////		}
////		default:{
////			printf("error en la devolucion");
////			break;
////		}
////	}
////	}
//	return NULL;
//}
//
//void* hablar_con_cpu(){
////	pthread_t hilo1;
////	pthread_t hilo2;
////	pthread_t hilo3;
////	pthread_create(&hilo1,NULL,interrupciones,NULL);
////	pthread_create(&hilo3,NULL,enviar_a_ejecutar_sjf,NULL);
////	pthread_create(&hilo2,NULL,devoluciones,NULL);
////	pthread_detach(hilo1);
////	pthread_detach(hilo2);
////	pthread_detach(hilo3);
////	return NULL;
//}
//
//bool menor_estimacion(void* pcb_aux,void* pcb_aux2){
////	pcb_t* pcb1 =pcb_aux;
////	pcb_t* pcb2 = pcb_aux2;
////	return pcb1->estimado_rafaga<pcb2->estimado_rafaga;
//}
//
//void mostrar_lista_ready_sjf(t_list* lista){
////	int i;
////	int lista_size= list_size(lista);
////	for(i=0;i<lista_size;i++){
////		pcb_t* pcb= malloc(sizeof(pcb_t));
////		pcb= list_get(lista,i);
////		printf("PCB = %d, prioridad= %d \n", pcb->pid,i);
////	}
//}
//
//void* agregar_a_ready_sjf(){
////	while(1){
////		sem_wait(&s_proceso_ready);
////		pcb_t* pcb;
////		if(!list_is_empty(interrumpidos_l)){
////			pthread_mutex_lock(&mx_interrumpidos_l);
////			pcb= list_remove(interrumpidos_l,0);
////			pthread_mutex_unlock(&mx_interrumpidos_l);
////		}
////		if(!list_is_empty(unblock_l)){
////			pthread_mutex_lock(&mx_unblock_l);
////			pcb= list_remove(unblock_l,0);
////			pthread_mutex_unlock(&mx_unblock_l);
////		}
////		if(!list_is_empty(susp_ready_l)){
////			pthread_mutex_lock(&mx_susp_ready_l);
////			pcb= list_remove(susp_ready_l,0);
////			pthread_mutex_unlock(&mx_susp_ready_l);
////		}
////		if(!list_is_empty(new_l)){
////			pthread_mutex_lock(&mx_new_l);
////			pcb= list_remove(new_l,0);
////			pthread_mutex_unlock(&mx_new_l);
////		}
////		pthread_mutex_lock(&mx_ready_l);
////		list_add_sorted(ready_l, pcb, menor_estimacion);
////		pthread_mutex_unlock(&mx_ready_l);
////		mostrar_lista_ready_sjf(ready_l);
////
////	}
////	return NULL;
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
