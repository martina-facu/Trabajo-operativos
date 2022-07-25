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
			logger = log_create(fileName, processName, 1, LOG_LEVEL_TRACE);
			log_info(logger, "-------------------------");
			log_info(logger, "Iniciando log Proceso %s", processName);
			if(log_level == LOG_LEVEL_TRACE)
				log_info(logger, "Inicializo el nivel de log a TRACE");
			else if(log_level == LOG_LEVEL_DEBUG)
				log_info(logger, "Inicializo el nivel de log a DEBUG");
			else if(log_level == LOG_LEVEL_INFO)
				log_info(logger, "Inicializo el nivel de log a INFO");
			else if(log_level == LOG_LEVEL_WARNING)
				log_info(logger, "Inicializo el nivel de log a WARNING");
			else if(log_level == LOG_LEVEL_ERROR)
				log_info(logger, "Inicializo el nivel de log a ERROR");
			log_info(logger, "-------------------------");

			return logger;
		}
	}

	/*
	 *  Funcion: setearLogLevel
	 *  Entradas: 	t_config* configFile		Variable de archivo de configuracion para leer el nivel de log
	 *  			char* fileName				Archivo en el cual se guardara el log
	 *  			char* processName			Nombre del proceso para el log
	 *  Salidas: t_log*
	 *  Razon: 	Busco el nivel de log del archivo de configuracion e inicializo el mismo de acuerdo
	 *  		a lo leido.
	 *  		De no existir el parametro en el archivo, seteo por defecto LOG_LEVEL_TRACE
	 *  Author: Grupo 99
	 */

	t_log* setearLogLevel(t_config* configFile, char* fileName, char* processName )
	{
		t_log* logger;
		//	Variable para leer el nivel de log deseado.
		char* logLevel;
		//	Busco el parametro LOG_LEVEL en el archivo de configuracion
		//	Si no existe seteo el log en TRACE, de existir en lo que diga la variable.

		logLevel = config_get_string_value(configFile, "LOG_LEVEL");
		if(logLevel == NULL)
		{
			logLevel = malloc(16*sizeof(char));
			strcpy(logLevel,"LOG_LEVEL_TRACE");
		}

		if(strcmp(logLevel,"LOG_LEVEL_TRACE")==0)
			logger = initLogger(fileName, processName, LOG_LEVEL_TRACE);
		else if(strcmp(logLevel,"LOG_LEVEL_DEBUG")==0)
			logger = initLogger(fileName, processName, LOG_LEVEL_DEBUG);
		else if(strcmp(logLevel,"LOG_LEVEL_INFO")==0)
			logger = initLogger(fileName, processName, LOG_LEVEL_INFO);
		else if(strcmp(logLevel,"LOG_LEVEL_WARNING")==0)
			logger = initLogger(fileName, processName, LOG_LEVEL_WARNING);
		else if(strcmp(logLevel,"LOG_LEVEL_ERROR")==0)
			logger = initLogger(fileName, processName, LOG_LEVEL_ERROR);
		else
			logger = initLogger(fileName, processName, LOG_LEVEL_TRACE);
		free(logLevel);

		return logger;

	}


