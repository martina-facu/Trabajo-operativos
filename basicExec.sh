set -x
#	Cargo las variables de entorno que voy a utilizar
export WORKSPACEPATH=$PWD
export MEMORYPATH=$WORKSPACEPATH/Memoria
export CPUPATH=$WORKSPACEPATH/CPU
export KERNELPATH=$WORKSPACEPATH/Kernel
export CONSOLEPATH=$WORKSPACEPATH/Consola
export CONFPATH=$WORKSPACEPATH/confPruebas
export LD_LIBRARY_PATH=$WORKSPACEPATH/Shared

#	Copio los archivos de configuracion BASE
cp $CONFPATH/memoria.config.base $MEMORYPATH/memoria.config
cp $CONFPATH/cpu.config.base $CPUPATH/cpu.config
cp $CONFPATH/kernel.config.base $KERNELPATH/kernel.config
cp $CONFPATH/consola.config.base $CONSOLEPATH/consola.config

#	Ejecuto primero memoria
cd $MEMORYPATH/Debug
$MEMORYPATH/Debug/Memoria &

#	Ejecuto segundo la CPU
cd $CPUPATH/Debug
sleep 1
$CPUPATH/Debug/CPU &

#	Ejecuto tercero el Kernel
cd $KERNELPATH/Debug
sleep 1
$KERNELPATH/Debug/Kernel &

#	Levanto la primera consola
cd $CONSOLEPATH/Debug
sleep 1
($CONSOLEPATH/Debug/Consola $CONFPATH/BASE_1 1024)  &

#	Levanto la segunda consola
sleep 1
($CONSOLEPATH/Debug/Consola $CONFPATH/BASE_2 1024)  &

#	Levanto la tercera consola
sleep 1
($CONSOLEPATH/Debug/Consola $CONFPATH/BASE_2 1024)  &

