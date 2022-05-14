#include "servidor.h"

void sigchld_handler(int s) {
	while (wait(NULL) > 0);
}

void atender(void* idSocketCliente) {
	int fdCliente = ((int *)idSocketCliente)[0];

	void* bufferEnvio = malloc(sizeof(char)*29);
	char * mensajeSaludoEnviado = malloc(sizeof(char)*29);
	strcpy(mensajeSaludoEnviado,"Hola CONSOLA, soy el KERNEL");
	mensajeSaludoEnviado[strlen(mensajeSaludoEnviado)] = '\0';
	memcpy(bufferEnvio,mensajeSaludoEnviado,29);
	if(send(fdCliente, bufferEnvio,sizeof(char)*29, 0)== -1){
		perror("recv");
		printf("No se pudo enviar saludo a la CONSOLA\n");
		exit(1);
	}
	printf("Saludo al ESI enviado correctamente\n");

	void* bufferRecibido = malloc(sizeof(char)*29);
	char * mensajeSaludoRecibido = malloc(sizeof(char)*29);
	int numbytes = 0;
	if ((numbytes = recv(fdCliente, bufferRecibido, 29, 0)) == -1) {
		perror("recv");
		printf("No se pudo recibir saludo a la CONSOLA\n");
		exit(1);
	}
	memcpy(mensajeSaludoRecibido,bufferRecibido,29);
	printf("Saludo del ESI recibido: %s\n", mensajeSaludoRecibido);

	free(bufferEnvio);
	free(bufferRecibido);
	free(mensajeSaludoEnviado);
	free(mensajeSaludoRecibido);

	free((int *)idSocketCliente);

}

void levantar_servidor_kernel() {

	//busco la ip y puerto
	t_config* config = config_create("config.cfg");
	if(!config){
		perror("No encuentro el archivo config");
		exit(1);
	}
	int MYPORT = config_get_int_value(config,"PUERTO_ESCUCHA_CONEXION");
	config_destroy(config);

	int sockfd; // Escuchar sobre: sock_fd, nuevas conexiones sobre: idSocketCliente
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del idSocketCliente
	int sin_size;
	struct sigaction sa;
	int yes = 1;

	//1° CREAMOS EL SOCKET
	//sockfd: numero o descriptor que identifica al socket que creo
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		printf("Error al abrir el socket de escucha\n");
		exit(1);
	}
	printf("Se creo el socket %d\n", sockfd);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = PF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(MYPORT);    // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = inet_addr(MYIP); //INADDR_ANY (aleatoria) o 127.0.0.1 (local)
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	//2° Relacionamos los datos de my_addr <=> socket
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		printf("Fallo el bind\n");
		perror("bind");
		exit(1);
	}

	//3° Listen: se usa para dejar al socket escuchando las conexiones que se acumulan en una cola hasta que
	//la aceptamos
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		printf("Fallo el listen\n");
		exit(1);
	}
	printf("Socket escuchando!!!\n");

	//-------
	sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	//--------
	sin_size = sizeof(struct sockaddr_in);


//	while (1) {
//		int *idSocketCliente = (int *)malloc(sizeof(int32_t));
//		idSocketCliente[0] = -1; //TODO: que pasa con esta variable con varios hilos ?
//		if ((idSocketCliente[0] = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size))
//				== -1) {
//			perror("Error al usar accept");
//		}
//
//		//CREAMOS UN HILO PARA ATENDERLO
//		pthread_t punteroHilo;
//		pthread_create(&punteroHilo, NULL, (void*) atender, idSocketCliente);
//		//espero a q termine el hilo (para evitar quilombo por ahora)
//		pthread_join(punteroHilo,NULL);
//
//	}
}
