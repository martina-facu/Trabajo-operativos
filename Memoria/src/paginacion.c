#include "paginacion.h"

//CREAR
/*
void inicializacion_proceso(int socket_cliente){

	int size;
	char* buffer;
	int desp = 0;

	char* instrucciones = leer_string(buffer, &desp);

	int pId = leer_entero(buffer, &desp);

	int tamanioTotal = leer_entero(buffer, &desp);

	log_info(logger, "Iniciando proceso %d que pesa %d", pId, tamanioTotal);

	int puedeGuardar = iniciar_paginacion(pId, instrucciones, tamanioTotal);

	if (puedeGuardar == 1){
		enviar_ok(socket_cliente);
		log_info(logger, "Envio ok");
	}
	else{
		log_error(logger, "No hay lugar para alojar el proceso en memoria, lola");
		enviar_fail(socket_cliente);
	}

	free(buffer);
	free(instrucciones);

}

t_tabla_pagina* crear_tabla_de_paginas(int pid, int size){

	t_tabla_pagina* unaTabla = malloc(sizeof(t_tabla_pagina));

	unaTabla->pid = pid;
	unaTabla->tamanioInstrucciones = size;
	//unaTabla->dlPcb = dlPcb;
	unaTabla->paginas_primer_nivel = list_create();
	unaTabla->paginas_segundo_nivel = list_create();

	//VER ESTO de los pthread

	pthread_mutex_lock(&mutexListaTablas);
	list_add(tablaDePaginas, unaTabla);
	pthread_mutex_unlock(&mutexListaTablas);

	return unaTabla;
}



int iniciar_paginacion(int pid, char* instrucciones, int tamanioTotal){

	int paginasNecesarias = ceil((double) tamanioTotal/ (double) config->page_size);

	int tamanioInstrucciones = (strlen(instrucciones) + 1);

	if(puedo_guardar_paginacion(paginasNecesarias)){ //SI HAY LUGAR EN MP O MV

        //PRIMERO GUARDAMOS LAS TAREAS
        t_list* paginasInstrucciones = guardar_instrucciones_paginacion(instrucciones);

        void mostrarId(t_pagina* pag)
        {
        log_info(logger, "%d",pag->idPagina);
        }

        log_info(logger, "Guarde las instrucciones del proceso %d en las paginas: ",pid);
        list_map(paginasInstrucciones, (void*)mostrarId);

        //DIRECCION LOGICA DE LAS INSTRUCCIONES PARA GUARDARLA EN LA PCB

        t_pagina* primerPagina= list_get(paginasInstrucciones,0);
        int dirInstrucciones = calcular_dir_logica(primerPagina,0);

        //CREAMOS LA TABLA DE PAGINAS
        t_tabla_pagina* unaTabla = crear_tabla_de_paginas(pid, tamanioInstrucciones);

        //GUARDO LAS TAREAS EN LA TABLA DEL PROCESO CON EL ID PATOTA
        agregar_paginas_a_tabla_de_proceso(unaTabla, paginasInstrucciones);

        //CREAMOS EL PCB

        Pcb* pcb = malloc(sizeof(Pcb));
        pcb->pid = pid;
        pcb->instrucciones = dirInstrucciones; //DONDE INICIAN LAS Instrucciones

        //GUARDAMOS LA PCB
        guardar_pcb_paginacion(pcb);

        free(pcb);

        return 1;

	}else{
		return 0;
	}

}

void agregar_paginas_a_tabla_de_proceso(t_tabla_pagina* unaTabla, t_list* paginasInstrucciones){

	agregar_paginas_a_tabla(unaTabla, paginasInstrucciones);

	list_destroy(paginasInstrucciones);
}

void agregar_paginas_a_tabla(t_tabla_pagina* unaTabla, t_list* paginas){
//capaz con un if
	list_add_all(unaTabla->paginas_primer_nivel, paginas);
	list_add_all(unaTabla->paginas_segundo_nivel, paginas);
}


t_list* guardar_instrucciones_paginacion(char* instrucciones){

	t_list* paginasQueOcupe;

	int tamanio = strlen(instrucciones) + 1;

	paginasQueOcupe = guardar_elementos_paginacion(instrucciones, tamanio);

	return paginasQueOcupe; //DEVUELVE LAS PAGINAS EN DONDE SE GUARDARON LAS INSTRUCCONES
}

void guardar_pcb_paginacion(Pcb* pcb){

	int desplazamiento = 0;

	t_tabla_pagina* tabla = buscar_tabla_pagina(pcb->pid);

	t_pagina* pagina = guardar_algo(pcb, 8, tabla, &desplazamiento); //pq 8?? es el tamanio

	log_info(logger, "Guarde la pcb del pid %d en las paginas %d y su desplazamiento es %d", tabla->pid, pagina->idPagina, desplazamiento);

	tabla->dlPcb = calcular_dir_logica(pagina, desplazamiento);

}

t_pagina* guardar_algo(void* algo, int size, t_tabla_pagina* tabla, int* desplazamiento){

	t_list* paginas;
	t_pagina* primerPag;

	//REVISAMOS SI CUANDO GUARDAMOS LAS INSTRUCCIONES SOBRO LUGAR EN LAS PAGINAS DE LA TABLA

	if(hay_lugar_en_las_paginas(tabla)){
		log_error(logger, "No hay lugar para alojar el proceso en memoria, lola");
		enviar_fail(socket_cliente);
	}

	free(buffer);
	free(instrucciones);

}

t_tabla_pagina* crear_tabla_de_paginas(int pid, int size){

	t_tabla_pagina* unaTabla = malloc(sizeof(t_tabla_pagina));

	unaTabla->pid = pid;
	unaTabla->tamanioInstrucciones = size;
	//unaTabla->dlPcb = dlPcb;
	unaTabla->paginas_primer_nivel = list_create();
	unaTabla->paginas_segundo_nivel = list_create();

	//VER ESTO de los pthread

	pthread_mutex_lock(&mutexListaTablas);
	list_add(tablaDePaginas, unaTabla);
	pthread_mutex_unlock(&mutexListaTablas);

	return unaTabla;
}



int iniciar_paginacion(int pid, char* instrucciones, int tamanioTotal){

	int paginasNecesarias = ceil((double) tamanioTotal/ (double) config->page_size);

	int tamanioInstrucciones = (strlen(instrucciones) + 1);

	if(puedo_guardar_paginacion(paginasNecesarias)){ //SI HAY LUGAR EN MP O MV

        //PRIMERO GUARDAMOS LAS TAREAS
        t_list* paginasInstrucciones = guardar_instrucciones_paginacion(instrucciones);

        void mostrarId(t_pagina* pag)
        {
        log_info(logger, "%d",pag->idPagina);
        }

        log_info(logger, "Guarde las instrucciones del proceso %d en las paginas: ",pid);
        list_map(paginasInstrucciones, (void*)mostrarId);

        //DIRECCION LOGICA DE LAS INSTRUCCIONES PARA GUARDARLA EN LA PCB

        t_pagina* primerPagina= list_get(paginasInstrucciones,0);
        int dirInstrucciones = calcular_dir_logica(primerPagina,0);

        //CREAMOS LA TABLA DE PAGINAS
        t_tabla_pagina* unaTabla = crear_tabla_de_paginas(pid, tamanioInstrucciones);

        //GUARDO LAS TAREAS EN LA TABLA DEL PROCESO CON EL ID PATOTA
        agregar_paginas_a_tabla_de_proceso(unaTabla, paginasInstrucciones);

        //CREAMOS EL PCB

        Pcb* pcb = malloc(sizeof(Pcb));
        pcb->pid = pid;
        pcb->instrucciones = dirInstrucciones; //DONDE INICIAN LAS Instrucciones

        //GUARDAMOS LA PCB
        guardar_pcb_paginacion(pcb);

        free(pcb);

        return 1;

	}else{
		return 0;
	}

}

void agregar_paginas_a_tabla_de_proceso(t_tabla_pagina* unaTabla, t_list* paginasInstrucciones){

	agregar_paginas_a_tabla(unaTabla, paginasInstrucciones);

	list_destroy(paginasInstrucciones);
}

void agregar_paginas_a_tabla(t_tabla_pagina* unaTabla, t_list* paginas){
//capaz con un if
	list_add_all(unaTabla->paginas_primer_nivel, paginas);
	list_add_all(unaTabla->paginas_segundo_nivel, paginas);
}


t_list* guardar_instrucciones_paginacion(char* instrucciones){

	t_list* paginasQueOcupe;

	int tamanio = strlen(instrucciones) + 1;

	paginasQueOcupe = guardar_elementos_paginacion(instrucciones, tamanio);

	return paginasQueOcupe; //DEVUELVE LAS PAGINAS EN DONDE SE GUARDARON LAS INSTRUCCONES
}

void guardar_pcb_paginacion(Pcb* pcb){

	int desplazamiento = 0;

	t_tabla_pagina* tabla = buscar_tabla_pagina(pcb->pid);

	t_pagina* pagina = guardar_algo(pcb, 8, tabla, &desplazamiento); //pq 8?? es el tamanio

	log_info(logger, "Guarde la pcb del pid %d en las paginas %d y su desplazamiento es %d", tabla->pid, pagina->idPagina, desplazamiento);

	tabla->dlPcb = calcular_dir_logica(pagina, desplazamiento);

}

t_pagina* guardar_algo(void* algo, int size, t_tabla_pagina* tabla, int* desplazamiento){

	t_list* paginas;
	t_pagina* primerPag;

	//REVISAMOS SI CUANDO GUARDAMOS LAS INSTRUCCIONES SOBRO LUGAR EN LAS PAGINAS DE LA TABLA

	if(hay_lugar_en_las_paginas(tabla)){

		t_list* paginasDisp = buscar_paginas_disponibles_en(tabla);

		primerPag = list_get(paginasDisp, 0);

		paginas = completar_espacio_en_la_pagina(algo, primerPag, size, desplazamiento);

		//GUARDO LA PCB EN EL LUGAR DE LA PAGINA (8 bytes)

		list_destroy(paginasDisp);

}
	else{ // SI NO SOBRO ESPACIO EN NINGUNA PAGINA, CREO UNA NUEVA Y LA AGREGO A LA TABLA

		paginas = guardar_elemento_paginacion(algo, size);

		primerPag = list_get(paginas, 0);

}

//SI SE CREO UNA PAGINA NUEVA
	if (!(list_is_empty(paginas)))
			agregar_paginas_a_tabla(tabla, paginas);

	list_destroy(paginas);

	return primerPag;
}

t_tabla_pagina* buscar_tabla_pagina(int pid){

	t_tabla_pagina* unaTabla;

	int mismoId(t_tabla_pagina* tabla)
	{
		return (tabla->pid == pid);
	}

    pthread_mutex_lock(&mutexListaTablas);
    unaTabla = list_find(tablaDePaginas, (void*)mismoId);
    pthread_mutex_unlock(&mutexListaTablas);

    return unaTabla;
}

t_list* guardar_elemento_paginacion(void* algo, int tamanio){

	//GUARDA PAGINAS NUEVAS -> VACIAS

	t_list* paginasQueOcupe = list_create();

	//BUSCO FRAMES LIBRES

	t_list* framesSinOcupar = buscar_frames_sin_ocupar(1); //ver si sacar MEM_PPAL

	int tamanioPag = config->page_size;

	if(list_is_empty(framesSinOcupar)){ //SI NO HAY FRAMES LIBRES -> BUSCO EN SWAP

		//VEO LA CANTIDAD DE PAGINAS QUE NECESITO
		int cantPag = ceil((double) tamanio / (double) tamanioPag);

		//HAGO SWAP DE ESA CANTIDAD

		swap(cantPag);

		//VUELVO A ITERAR LA FUN
		t_list* paginasDisp = buscar_paginas_disponibles_en(tabla);

		primerPag = list_get(paginasDisp, 0);

		paginas = completar_espacio_en_la_pagina(algo, primerPag, size, desplazamiento);

		//GUARDO LA PCB EN EL LUGAR DE LA PAGINA (8 bytes)

		list_destroy(paginasDisp);

}
	else{ // SI NO SOBRO ESPACIO EN NINGUNA PAGINA, CREO UNA NUEVA Y LA AGREGO A LA TABLA

		paginas = guardar_elemento_paginacion(algo, size);

		primerPag = list_get(paginas, 0);

}

//SI SE CREO UNA PAGINA NUEVA
	if (!(list_is_empty(paginas)))
			agregar_paginas_a_tabla(tabla, paginas);

	list_destroy(paginas);

	return primerPag;
}

t_tabla_pagina* buscar_tabla_pagina(int pid){

	t_tabla_pagina* unaTabla;

	int mismoId(t_tabla_pagina* tabla)
	{
		return (tabla->pid == pid);
	}

    pthread_mutex_lock(&mutexListaTablas);
    unaTabla = list_find(tablaDePaginas, (void*)mismoId);
    pthread_mutex_unlock(&mutexListaTablas);

    return unaTabla;
}

t_list* guardar_elemento_paginacion(void* algo, int tamanio){

	//GUARDA PAGINAS NUEVAS -> VACIAS

	t_list* paginasQueOcupe = list_create();

	//BUSCO FRAMES LIBRES

	t_list* framesSinOcupar = buscar_frames_sin_ocupar(1); //ver si sacar MEM_PPAL

	int tamanioPag = config->page_size;

	if(list_is_empty(framesSinOcupar)){ //SI NO HAY FRAMES LIBRES -> BUSCO EN SWAP

		//VEO LA CANTIDAD DE PAGINAS QUE NECESITO
		int cantPag = ceil((double) tamanio / (double) tamanioPag);

		//HAGO SWAP DE ESA CANTIDAD

		swap(cantPag);

		//VUELVO A ITERAR LA FUNCION CON EL SWAP HECHO

		t_list* nuevasPaginas = guardar_elemento_paginacion(algo, tamanio);

		list_add_all(paginasQueOcupe, nuevasPaginas);
		list_destroy(nuevasPaginas);
	}
	else{
		//HAY FRAMES LIBES EN PPAL

		if(tamanio <= tamanioPag){ //SI LO QUE TENGO QUE GUARDAR ENTRA EN UNA PAGINA

			t_frame* frame = list_get(framesSinOcupar, 0); //AGARRO EL PRIMERO QUE ENCUENTRO EN LA LISTA DE LIBRES

			guardar_en_memoria_paginacion(algo, tamanio, frame, 1);

			t_pagina* pagina = crearPagina(frame, (tamanioPag - tamanio)); //Si las instrucciones ocupen menos que la pag me van a sobrar bytes

			list_add(paginasQueOcupe, pagina);

		}else{

			//SI NECESITO MAS PAGINAS

			int sobrante = tamanio - tamanioPag; //lo que no me entra en la primer pagina

			t_list* pagina = guardar_elemento_paginacion(algo, tamanioPag); // Guardo lo primero
			t_list* restoPaginas = guardar_elemento_paginacion(algo + tamanioPag, sobrante);

			list_add_all(paginasQueOcupe, pagina);

			list_destroy(pagina);
			list_destroy(restoPaginas);
		}

	}

	eliminar_lista(framesSinOcupar);
	return paginasQueOcupe;
}
/*
void guardar_en_memoria_paginacion(void* algo, int tamanio, t_frame* unFrame, int mem){

	ocupar_frame(unFrame, mem);
	ocupar_memoria_paginacion(algo, tamanio, unFrame->idFrame, 0, mem); //mem será MEM_PPAL?
	//LE PASO 0 pq es una pagina vacia
}

void ocupar_memoria_paginacion(void* algo, int tamanio, int idFrame, int desplazamiento, int mem){

	//BASE DEL FRAME = ID * TAMANIO_PAGINA + DESPLAZAMIENTO (dentro de la pag) -> el ID Indica el nro de frame

	int base = ((idFrame * config->page_size) + desplazamiento);

	if(mem == MEM_PPAL){
		pthread_mutex_lock(&mutexMemoria);
		memcpy(memoriaPrincipal + base, algo, tamanio);
		pthread_mutex_unlock(&mutexMemoria);


}

}

*/
//ELIMINAR
/*
void finalizar_proceso(int socket_cliente) {
	int size;
	char* buffer;
	int desp = 0;

	int pId = leer_entero(buffer, &desp);

int puedeEliminar = iniciar_eliminacion_proceso(pId);

	if (puedeEliminar == 1){
		enviar_ok(socket_cliente);
		log_info(logger, "Se finalizo el proceso ok");
	}
	else{
		log_error(logger, "Ocurrio un error al finalizar el proceso, lola");
		enviar_fail(socket_cliente);
	}
}
*/
//PARA TERMINA UN PROCESO TENEMOS QUE:
// 1) Buscar el proceso en memoria y libero el espacio utilizado
// 2) Eliminar pcb
// 3) Eliminar tablas? No es necesario para este tp
// 4) Eliminar archivo de swap

/*
int iniciar_eliminacion_proceso(int pid){

//Busco el proceso en la lista de espacio utilizado y lo elimino de la lista

int cantidadProcesos = list_size(espacioUtilizado);

for (int i = 0; i < cantidadProcesos; i++){
proceso = list_get(espacioUtilizado,i);

if (proceso->pid == pid){
limpiar_posiciones(espacio, proceso->posPagina, proceso->cantidadDePaginas);
list_remove_and_destroy_element(espacioUtilizado,i, free);
eliminar_proceso(unaTabla);
log_info(logger, "El proceso %d - Pagina inicial:%d Tamanio:%d fue eliminado correctamente", pid, proceso->posPagina, proceso->cantidadDePaginas);
break;//?
}
}

eliminar_archivo_swap(pid);

}

void limpiar_posiciones(t_bitarray* unEspacio, int posicionInicial, int tamanioProceso){

	int i = 0;
	for (i = posicionInicial; i < posicionInicial + tamanioProceso; i++){

		if(bitarray_test_bit(espacio, i) == 1)
			espacioDisponbile++;
		bitarray_clean_bit(unEspacio, i);
	}
}

void eliminar_proceso(t_tabla_pagina* unaTabla){

	//log_info(logger, "Se elimino el proceso %d",unaTabla->idPatota);

	//Libero las paginas
	list_iterate(unaTabla->paginas_primer_nivel,(void*)matar_pagina);
	list_iterate(unaTabla->paginas_segundo_nivel,(void*)matar_pagina);

	//Borro el quilombo

	int mismoId(t_tabla_pagina* tabla)
	{
		return unaTabla->pid == tabla->pid;
	}

	pthread_mutex_lock(&mutexListaTablas);
	list_remove_and_destroy_by_condition(tablaDePaginas, (void*) mismoId, (void*) eliminar_tabla_paginas);
	pthread_mutex_unlock(&mutexListaTablas);
}

void matar_pagina(t_pagina* unaPagina){

		//LA SACO DEL BITMAP
		desocupar_frame(unaPagina->frame_ppal);
}


void liberar_paginas(t_list* paginas, int size, t_tabla_pagina* unaTabla, int base, int relleno){

		if(list_size(paginas) > 1){ //OCUPO MAS DE UNA PAGINA

			if((base % config->page_size) == 0){ //SI ESTOY AL PRINCIPIO DE UN FRAME, LO LIBERO ENTERO
				liberar_pagina(list_get(paginas,0), unaTabla); //Como tengo mas de una pagina, y empiezo desde el inicio de la misma, 100% seguros que ocupa toda la misma
				list_remove(paginas,0);
				liberar_paginas(paginas, size - config->page_size, unaTabla, base + config->page_size, config->page_size);
			}else{ //SI HAY OTRAS COSAS EN LA PAGINA
				t_pagina* primerPag = list_get(paginas,0);
				primerPag->fragInterna += relleno; //el cachito que ocupaba de la pagina queda como frag interna
				if(primerPag->fragInterna == config->page_size){//Si el espacio libre mas lo que tengo mas la frag interna es un frame, NO HAY NADA MAS EN LA PAG Y LA PUEO LIBERAR
					liberar_pagina(primerPag, unaTabla);
				}
				list_remove(paginas,0);
				liberar_paginas(paginas, size - relleno, unaTabla, base + relleno, config->page_size);//Muevo la base el sobramte, que es el cachito que ocupa
			}
		}else{ //Aca tengo que ver si ocupa toda la pagina o solo un cachito, pero nunca se sale de la misma
			t_pagina* unicaPag = list_get(paginas,0);

			if(unicaPag->fragInterna + unicaPag->tamanioDisponible + size == config->page_size) {//SI LA SUMA DE TODO ESO DA IGUAL AL TAM DE UNA PAGINA, ENTONCES SIGNIFICA QUE NO HAY NADA MAS Y LA PUEDO BORRAR
				liberar_pagina(unicaPag, unaTabla); //LIBERO LA PAGINA
			}
			else unicaPag->fragInterna += size; //LO Q "BORRE" (EL SIZE) QUEDA COMO FRA INTERNA
			//En el resto de los casos no me importa liberar la pagina, porque la ocupa otra cosa
		}
}

void liberar_pagina(t_pagina* unaPagina, t_tabla_pagina* unaTabla){

		//ANULO LA PAGINA
		unaPagina->idPagina = -1;//Asi no la encuentra nadie

		//LA SACO DEL BITMAP LA ESTRUCTURA DE PAGINA TIENE QUE TENER UN FRAME?
		desocupar_frame(unaPagina->frame_ppal);

		//ANULO EL FRAME DE LA PAGINA
		unaPagina->frame_ppal = -1;

		//unaPagina->tamanioDisponible = 0;
}

void desocupar_frame(int frame){

	pthread_mutex_lock(&mutexBitmapMP);
	bitarray_clean_bit(marcosOcupadosPpal, frame);
	pthread_mutex_unlock(&mutexBitmapMP);
}

void eliminar_tabla_paginas(t_tabla_pagina* unaTabla){

	eliminar_lista(unaTabla->paginas_primer_nivel);
	eliminar_lista(unaTabla->paginas_segundo_nivel);
	//eliminar_lista(unaTabla->pid);
	free(unaTabla);
}

void eliminar_lista(t_list* lista){

	list_destroy_and_destroy_elements(lista, (void*)eliminar_algo);
}

void eliminar_algo(void* algo){
	free(algo);
}
*/
