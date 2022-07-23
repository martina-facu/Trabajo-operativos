#set -x
#	Cargo las variables de entorno que voy a utilizar
cd ..
export WORKSPACEPATH=$PWD
export MEMORYPATH=$WORKSPACEPATH/Memoria
export CPUPATH=$WORKSPACEPATH/CPU
export KERNELPATH=$WORKSPACEPATH/Kernel
export CONSOLEPATH=$WORKSPACEPATH/Consola
export CONFPATH=$WORKSPACEPATH/confPruebas
export LD_LIBRARY_PATH=$WORKSPACEPATH/Shared

#	Obtengo IP del equipo
READIP=$(hostname -I | cut -f1 -d' ')

#	Pregunto si la IP obtenida es la correcta y de no serlo pide agregarlas a mano.
while true; do
    read -p "La IP de la Memoria es: $READIP. Ingrese S si es correcto o N para ingresar una nueva IP.  " yn
    case $yn in
	    [Ss]* ) export IPMEMORIA=$READIP; break;;
        [Nn]* ) read -p "Ingrese la IP de la Memoria: " IPMEMORIA; break;;
        * ) echo "Ingrese Si o No.";;
    esac
done

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
aplicaciones[11]="Matar_Procesos"
aplicaciones[12]="Ver_Procesos"
aplicaciones[13]="Salir"

select appl in ${aplicaciones[*]}
do
	case $appl in
	'Salir')	exit;;
		'Prueba_Base')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=4096" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=64" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=4" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=2000" >> $MEMORYPATH/memoria.config
			
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		
		'Prueba_PlanificacionFIFO')			
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=4096" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=64" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=4" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=2000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_PlanificacionSRT')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=4096" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=64" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=4" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=2000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_SuspensionFIFO')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=4096" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=64" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=4" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=2000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_SuspensionSRT')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=4096" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=64" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=4" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=2000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_MemoriaClock')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=2048" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=256" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=3" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=5000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_Memoria_Clock-M')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=2048" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=256" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=1000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=3" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=5000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_TLB-FIFO')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=8192" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=128" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=3000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=16" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=3000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_TLB-LRU')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=8192" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=128" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=3000" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=16" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=3000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Prueba_Integral')	
			#	Armo el archivo de configuracion BASE
			echo "IPMEMORIA=$IPMEMORIA" > $MEMORYPATH/memoria.config
			echo "PUERTO_ESCUCHA=8002" >> $MEMORYPATH/memoria.config
			echo "ENTRADAS_POR_TABLA=4" >> $MEMORYPATH/memoria.config
			echo "PATH_SWAP=/home/utnso/swap" >> $MEMORYPATH/memoria.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $MEMORYPATH/memoria.config
			echo "TAM_MEMORIA=10240" >> $MEMORYPATH/memoria.config
			echo "TAM_PAGINA=256" >> $MEMORYPATH/memoria.config			
			echo "RETARDO_MEMORIA=500" >> $MEMORYPATH/memoria.config
			echo "ALGORITMO_REEMPLAZO=CLOCK-M" >> $MEMORYPATH/memoria.config
			echo "MARCOS_POR_PROCESO=4" >> $MEMORYPATH/memoria.config
			echo "RETARDO_SWAP=1000" >> $MEMORYPATH/memoria.config
			
			#	Ejecuto la Memoria
			cd $MEMORYPATH
			$MEMORYPATH/Memoria &
		;;
		'Matar_Procesos')	
			$WORKSPACEPATH/scripts/kpsTP.sh &
		;;
		'Ver_Procesos')	
			$WORKSPACEPATH/scripts/psTP.sh &
		;;
		*)
			
	esac
done






