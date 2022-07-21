set -x

#	Retrocedo un nivel para poder setear el workspace
cd ..
export WORKSPACE=$PWD
#	Seteo las variables de ambiente
export MEMORIA=$WORKSPACE/Memoria
export KERNEL=$WORKSPACE/Kernel
export CPU=$WORKSPACE/CPU
export CONSOLA=$WORKSPACE/Consola
export CONFENV=$WORKSPACE/confPruebas

#	Realizo la copia con el nombre correcto de cada config utilizando el base como parametro.
cp $CONFENV/memoria.config.base $MEMORIA/memoria.config
cp $CONFENV/kernel.config.base $KERNEL/kernel.config
cp $CONFENV/cpu.config.base $CPU/cpu.config
cp $CONFENV/consola.config.base $CONSOLA/consola.config
