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

#	Obtengo IP del equipo
READIP=$(hostname -I | cut -f1 -d' ')

#	Pregunto si la IP obtenida es la correcta y de no serlo pide agregarlas a mano.
while true; do
    read -p "La IP de la CPU es: $READIP. Ingrese S si es correcto o N para ingresar una nueva IP.  " yn
    case $yn in
        [Ss]* ) export IP_CPU=$READIP; break;;
        [Nn]* ) read -p "Ingrese la IP del CPU: " IP_CPU; break;;
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
aplicaciones[12]="Salir"

select appl in ${aplicaciones[*]}
do
	case $appl in
	'Salir')	exit;;
		'Prueba_Base')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		
		'Prueba_PlanificacionFIFO')			
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_PlanificacionSRT')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_SuspensionFIFO')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_SuspensionSRT')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_MemoriaClock')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=1" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_Memoria_Clock-M')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=1" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_TLB-FIFO')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=FIFO" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_TLB-LRU')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=4" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Prueba_Integral')	
			#	Armo el archivo de configuracion BASE
			echo "IP_CPU=$IP_CPU" > $CPUPATH/cpu.config
			echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
			echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
			echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
			echo "LOG_LEVEL=LOG_LEVEL_INFO" >> $CPUPATH/cpu.config
			echo "ENTRADAS_TLB=2" >> $CPUPATH/cpu.config
			echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
			echo "RETARDO_NOOP=100" >> $CPUPATH/cpu.config
			
			#	Ejecuto la CPU
			cd $CPUPATH
			$CPUPATH/CPU &
		;;
		'Matar_Procesos')	
			$(WORKSPACEPATH/scripts/kpsTP.sh) &
		;;
		*)
			
	esac
done






