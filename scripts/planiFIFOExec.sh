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

#	Copio los archivos de configuracion BASE
cp $CONFPATH/memoria.config.kplanfifo $MEMORYPATH/memoria.config
cp $CONFPATH/cpu.config.kplanfifo $CPUPATH/cpu.config
cp $CONFPATH/kernel.config.kplanfifo $KERNELPATH/kernel.config
cp $CONFPATH/consola.config.kplanfifo $CONSOLEPATH/consola.config

#	Ejecuto primero memoria
cd $MEMORYPATH
$MEMORYPATH/Memoria &

#	Ejecuto segundo la CPU
cd $CPUPATH
sleep 2
$CPUPATH/CPU &

#	Ejecuto tercero el Kernel
cd $KERNELPATH
sleep 2
$KERNELPATH/Kernel &

#	Levanto la primera consola
cd $CONSOLEPATH
sleep 1
($CONSOLEPATH/Consola $CONFPATH/PLANI_1 1024)  &

#	Levanto la segunda consola
($CONSOLEPATH/Consola $CONFPATH/PLANI_1 1024)  &

#	Levanto la tercera consola
($CONSOLEPATH/Consola $CONFPATH/PLANI_2 1024)  &

