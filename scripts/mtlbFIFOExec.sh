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
cp $CONFPATH/memoria.config.mtlbfifo $MEMORYPATH/memoria.config
cp $CONFPATH/cpu.config.mtlbfifo $CPUPATH/cpu.config
cp $CONFPATH/kernel.config.mtlbfifo $KERNELPATH/kernel.config
cp $CONFPATH/consola.config.mtlbfifo $CONSOLEPATH/consola.config

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
($CONSOLEPATH/Consola $CONFPATH/TLB_1   2048)  &

#	Levanto la segunda consola
($CONSOLEPATH/Consola $CONFPATH/TLB_2  2048)  &