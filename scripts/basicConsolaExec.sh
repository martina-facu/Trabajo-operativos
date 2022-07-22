set -x
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
