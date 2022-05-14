#include "cliente.h"

void saludo_inicial_kernel(int sockfd){
	//Recibo saludo del Kernel
	void* bufferRecibido = malloc(sizeof(char)*29);
	char * mensajeSaludoRecibido = malloc(sizeof(char)*29);
	int numbytes = 0;
	if ((numbytes = recv(sockfd, bufferRecibido, 29, 0)) == -1) {
		perror("recv");
		printf("No se pudo recibir saludo al kernel\n");
		exit(1);
	}
	memcpy(mensajeSaludoRecibido,bufferRecibido,29);
	printf("Saludo de la consola recibido: %s\n", mensajeSaludoRecibido);

	//Envio saludo al Kernel
	void* bufferEnvio = malloc(sizeof(char)*29);
	char * mensajeSaludoEnviado = malloc(sizeof(char)*29);
	strcpy(mensajeSaludoEnviado,"Hola kernel, soy la consola");
	mensajeSaludoEnviado[strlen(mensajeSaludoEnviado)] = '\0';
	memcpy(bufferEnvio,mensajeSaludoEnviado,29);

	if(send(sockfd, bufferEnvio , 29, 0)== -1){
		perror("recv");
		exit(1);
	}
	printf("Envie mi saludo al kernel exitosamente\n");


	free(bufferRecibido);
	free(mensajeSaludoEnviado);
	free(bufferEnvio);
	free(mensajeSaludoRecibido);

}



int conectar_kernel(){
	//busco la ip y puerto
	t_config* config = config_create("config.cfg");
	if(!config){
		perror("No encuentro el archivo config");
		exit(1);
	}
	int PORT = config_get_int_value(config,"PUERTO_CONFIG_KERNEL");
	char * IP = malloc(sizeof(char)*100);
	strcpy(IP,config_get_string_value(config,"IP_CONFIG_KERNEL"));
	config_destroy(config);

	//Creamos un socket
	int sockfd;
    struct sockaddr_in their_addr; // información de la dirección de destino

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;    // Ordenación de bytes de la máquina
    their_addr.sin_port = htons(PORT);  // short, Ordenación de bytes de la red
    their_addr.sin_addr.s_addr = inet_addr(IP);//toma la ip directo

    memset( &(their_addr.sin_zero) , 0 , 8);  // poner a cero el resto de la estructura

    if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
        perror("No se pudo conectar");
        exit(1);
    }

    free(IP);
    return(sockfd);
}
