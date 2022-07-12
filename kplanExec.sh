set -x
#	Cargo las variables de entorno que voy a utilizar
export WORKSPACEPATH=$PWD
export MEMORYPATH=$WORKSPACEPATH/Memoria
export CPUPATH=$WORKSPACEPATH/CPU
export KERNELPATH=$WORKSPACEPATH/Kernel
export CONSOLEPATH=$WORKSPACEPATH/Consola
export CONFPATH=$WORKSPACEPATH/confPruebas
export LD_LIBRARY_PATH=$WORKSPACEPATH/Shared/Debug

#	Copio los archivos de configuracion BASE
cp $CONFPATH/memoria.config.kplan $MEMORYPATH/Debug/memoria.config
cp $CONFPATH/cpu.config.kplan $CPUPATH/Debug/cpu.config
cp $CONFPATH/kernel.config.kplan $KERNELPATH/Debug/kernel.config
cp $CONFPATH/consola.config.kplan $CONSOLEPATH/Debug/consola.config

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
($CONSOLEPATH/Debug/Consola $CONFPATH/PLANI_1 1024)  &

#	Levanto la segunda consola
sleep 1
($CONSOLEPATH/Debug/Consola $CONFPATH/PLANI_1 1024)  &

#	Levanto la tercera consola
sleep 1
($CONSOLEPATH/Debug/Consola $CONFPATH/PLANI_2 1024)  &

