set -o vi
#       Declaro variables de entorno
export TPHOME=$HOME/tp-2022-1c-9-12
export SHAREDCOMPILAR=$TPHOME/Shared/compilar
export SHARED=$TPHOME/Shared
export CONSOLACOMPILAR=$TPHOME/Consola/compilar
export KERNELCOMPILAR=$TPHOME/Kernel/compilar
export CPUCOMPILAR=$TPHOME/CPU/compilar
export MEMORIACOMPILAR=$TPHOME/Memoria/compilar
export CONSOLA=$TPHOME/Consola
export KERNEL=$TPHOME/Kernel
export CPU=$TPHOME/CPU
export MEMORIA=$TPHOME/Memoria
cd $TPHOME
git pull

#       Me posiciono en la shared library de nuestro TP y compilo
cd $SHAREDCOMPILAR
make clean
make
mv libShared.so $SHARED/libShared.so
#       Me posiciono en la Consola de nuestro TP y compilo
cd $CONSOLACOMPILAR
make clean
make
mv Consola $CONSOLA/Consola
#       Me posiciono en el Kernel de nuestro TP y compilo
cd $KERNELCOMPILAR
make clean
make
mv Kernel $KERNEL/Kernel
#       Me posiciono en la CPU de nuestro TP y compilo
cd $CPUCOMPILAR
make clean
make
mv CPU $CPU/CPU
#       Me posiciono en la Memoria de nuestro TP y compilo
cd $MEMORIACOMPILAR
make clean
make
mv Memoria $MEMORIA/Memoria
