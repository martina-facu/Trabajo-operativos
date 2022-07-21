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
cp $CONFPATH/memoria.config.integral $MEMORYPATH/memoria.config
cp $CONFPATH/cpu.config.integral $CPUPATH/cpu.config
cp $CONFPATH/kernel.config.integral $KERNELPATH/kernel.config
cp $CONFPATH/consola.config.integral $CONSOLEPATH/consola.config

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
