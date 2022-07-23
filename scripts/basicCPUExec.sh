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

#	Armo el archivo de configuracion BASE
echo "ENTRADAS_TLB=4" > $CPUPATH/cpu.config
echo "REEMPLAZO_TLB=LRU" >> $CPUPATH/cpu.config
echo "RETARDO_NOOP=1000" >> $CPUPATH/cpu.config
echo "PUERTO_MEMORIA=8002" >> $CPUPATH/cpu.config
echo "PUERTO_ESCUCHA_DISPATCH=8001" >> $CPUPATH/cpu.config
echo "PUERTO_ESCUCHA_INTERRUPT=8005" >> $CPUPATH/cpu.config
read -p "Ingrese la IP de la Memoria: " IPMEMORIA
echo "IP_MEMORIA=$IPMEMORIA" >> $CPUPATH/cpu.config
READIP=$(hostname -I | cut -f1 -d' ')
while true; do
    read -p "La IP de la CPU es: $READIP. Ingrese S si es correcto o N para ingresar una nueva IP.  " yn
    case $yn in
        [Ss]* ) echo "IP_CPU=$READIP" >> $CPUPATH/cpu.config; break;;
        [Nn]* ) read -p "Ingrese la IP del CPU: " IPCPU; echo "IP_CPU=$IPCPU" >> $CPUPATH/cpu.config; break;;
        * ) echo "Ingrese Si o No.";;
    esac
done

# #	Ejecuto segundo la CPU
cd $CPUPATH
$CPUPATH/CPU &


