#!/bin/bash
# set -x
#	Cargo las variables de entorno que voy a utilizar
cd ..
export WORKSPACEPATH=$PWD
export MEMORYPATH=$WORKSPACEPATH/Memoria
export CPUPATH=$WORKSPACEPATH/CPU
export KERNELPATH=$WORKSPACEPATH/Kernel
export CONSOLEPATH=$WORKSPACEPATH/Consola
export CONFPATH=$WORKSPACEPATH/confPruebas
export LD_LIBRARY_PATH=$WORKSPACEPATH/Shared

#	Armo el archivo de configuracion BASE
read -p "Ingrese la IP del Kernel: " IPKERNEL
echo "IP_KERNEL=$IPKERNEL" > $CONSOLEPATH/consola.config
echo "PUERTO_KERNEL=8000" >> $CONSOLEPATH/consola.config
echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CONSOLEPATH/consola.config

aplicaciones[1]="Prueba_Base"
aplicaciones[2]="Prueba_PlanificacionFIFO"
aplicaciones[3]="Prueba_PlanificacionSRT"
aplicaciones[4]="Prueba_SuspensionFIFO"
aplicaciones[5]="Prueba_SuspensionSRT"
aplicaciones[6]="Prueba_MemoriaClock"
aplicaciones[7]="Prueba_Memoria_Clock-M"
aplicaciones[8]="Prueba_TLB-FIFO"
aplicaciones[9]="Prueba_TLB-LRU"
aplicaciones[10]="Prueba_Integral"
aplicaciones[11]="Salir"

select appl in ${aplicaciones[*]}
do
	case $appl in
	'Salir')	exit;;
		'Prueba_Base')	
			# Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/BASE_1 1024)  &
			sleep 1
			# Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/BASE_2 1024)  &
			sleep 1
			# Levanto la tercera consola
			($CONSOLEPATH/Consola $CONFPATH/BASE_2 1024)  &
		;;
		'Prueba_PlanificacionFIFO')			
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/PLANI_1 1024)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/PLANI_1 1024)  &
			sleep 1
			#	Levanto la tercera consola
			($CONSOLEPATH/Consola $CONFPATH/PLANI_2 1024)  &
		;;
		'Prueba_PlanificacionSRT')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/PLANI_1 1024)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/PLANI_1 1024)  &
			sleep 1
			#	Levanto la tercera consola
			($CONSOLEPATH/Consola $CONFPATH/PLANI_2 1024)  &
		;;
		'Prueba_SuspensionFIFO')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/SUSPE_1 1024)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/SUSPE_2 1024)  &
			sleep 1
			#	Levanto la tercera consola
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/SUSPE_3 1024)  &
		;;
		'Prueba_SuspensionSRT')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/SUSPE_1 1024)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/SUSPE_2 1024)  &
			sleep 1
			#	Levanto la tercera consola
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/SUSPE_3 1024)  &
		;;
		'Prueba_MemoriaClock')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/MEMORIA_1  4096)  &
		;;
		'Prueba_Memoria_Clock-M')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/MEMORIA_1  4096)  &
		;;
		'Prueba_TLB-FIFO')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/TLB_1   2048)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/TLB_2  2048)  &
		;;
		'Prueba_TLB-LRU')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/TLB_1   2048)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/TLB_2  2048)  &
		;;
		'Prueba_Integral')	
			#	Levanto la primera consola
			cd $CONSOLEPATH
			sleep 1
			($CONSOLEPATH/Consola $CONFPATH/INTEGRAL_1 2048)  &
			sleep 1
			#	Levanto la segunda consola
			($CONSOLEPATH/Consola $CONFPATH/INTEGRAL_2 2048)  &
			sleep 1
			#	Levanto la tercera consola
			($CONSOLEPATH/Consola $CONFPATH/INTEGRAL_3 2048)  &
			sleep 1
			#	Levanto la cuarta consola
			($CONSOLEPATH/Consola $CONFPATH/INTEGRAL_4 2048)  &
			sleep 1
			#	Levanto la quinta consola
			($CONSOLEPATH/Consola $CONFPATH/INTEGRAL_5 2048)  &
		;;
		*)
			
	esac
done
