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

#	Ingreso la IP de la memoria
read -p "Ingrese la IP de la Memoria: " IPMEMORIA

#	Ingreso la IP de la CPU
read -p "Ingrese la IP de la CPU: " IPCPU

#	Obtengo IP del equipo
READIP=$(hostname -I | cut -f1 -d' ')

#	Pregunto si la IP obtenida es la correcta y de no serlo pide agregarlas a mano.
while true; do
    read -p "La IP del Kernelito es: $READIP. Ingrese S si es correcto o N para ingresar una nueva IP.  " yn
    case $yn in
        [Ss]* ) export IPKERNELITO=$READIP; break;;
        [Nn]* ) read -p "Ingrese la IP del Kernelito: " IPKERNELITO; break;;
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
			#	Armo el archivo de configuraci|on BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=10000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=4" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=100000" >> $KERNELPATH/kernel.config
			
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		
		'Prueba_PlanificacionFIFO')			
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config

			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=4" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=10000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_PlanificacionSRT')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=SRT" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=4" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=10000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_SuspensionFIFO')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=2" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=8000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_SuspensionSRT')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=SRT" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=2" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=8000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_MemoriaClock')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=2" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=5000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_Memoria_Clock-M')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=2" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=5000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_TLB-FIFO')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=2" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=5000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_TLB-LRU')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=FIFO" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=20000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.5" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=2" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=5000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto la Memoria
			cd $KERNELPATH
			$KERNELPATH/Kernel &
		;;
		'Prueba_Integral')	
			#	Armo el archivo de configuracion BASE
			echo "IP_KERNEL=$IPKERNELITO" > $KERNELPATH/kernel.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $KERNELPATH/kernel.config
			echo "IP_CPU=$IPCPU" >> $KERNELPATH/kernel.config
			echo "PUERTO_MEMORIA=8002" >> $KERNELPATH/kernel.config	
			echo "PUERTO_CPU_DISPATCH=8001" >> $KERNELPATH/kernel.config
			echo "PUERTO_CPU_INTERRUPT=8005" >> $KERNELPATH/kernel.config
			echo "PUERTO_ESCUCHA=8000" >> $KERNELPATH/kernel.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $KERNELPATH/kernel.config
			echo "ALGORITMO_PLANIFICACION=SRT" >> $KERNELPATH/kernel.config
			echo "ESTIMACION_INICIAL=50000" >> $KERNELPATH/kernel.config			
			echo "ALFA=0.2" >> $KERNELPATH/kernel.config
			echo "GRADO_MULTIPROGRAMACION=6" >> $KERNELPATH/kernel.config
			echo "TIEMPO_MAXIMO_BLOQUEADO=1000" >> $KERNELPATH/kernel.config
			
			#	Ejecuto el Kernelito
			cd $KERNELPATH
			$KERNELPATH/Kernel &
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
