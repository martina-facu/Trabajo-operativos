/*
 * logs.c
 *
 *  Created on: 1 may. 2022
 *      Author: utnso
 */



/*
 * Listado de librerias incluidas
 */

#include "logs.h"



/*
 *  Funciones
 */



	/*
	 *  Funcion: initLogger
	 *  Entradas: 	char* fileName
	 *  			char* processName
	 *  			t_log_level log_level
	 *  Salidas: t_log*
	 *  Razon: 	Inicializar la variable del archivo de configuracion alocando
	 *  		la memoria necesaria para su uso
	 *  Author: Grupo 99
	 */


	t_log* initLogger(char* fileName, char* processName, t_log_level log_level)
	{
		t_log* logger;
//		if((logger = log_create("cpu.log", "cpu", 1, log_level)) == NULL)
		if((logger = log_create(fileName, processName, 1, log_level)) == NULL)
		{

			printf("\nERROR NO se pudo crear el archivo de log del CPU!!!!!\n");
			exit(EXIT_FAILURE);

		}
		else{
			logger = log_create(fileName, processName, 1, LOG_LEVEL_INFO);
			log_info(logger, "-------------------------");
			log_info(logger, "Iniciando log Proceso %s", processName);
			log_info(logger, "-------------------------");

			return logger;
		}
	}




