#	Declaro variables de entorno
export TPHOME=$HOME/tp-2022-1c-9-12
export SHARED=$TPHOME/Shared
export CONSOLACOMPILAR=$TPHOME/Consola/compilar
export KERNELCOMPILAR=$TPHOME/Kernel/compilar
export CPUCOMPILAR=$TPHOME/CPU/compilar
export MEMORIA=$TPHOME/Memoria/compilar
export CONSOLA=$TPHOME/Consola
export KERNEL=$TPHOME/Kernel
export CPU=$TPHOME/CPU
export MEMORIA=$TPHOME/Memoria

#	Me posiciono sobre el home del usuario
cd
#	Clono la so-commons-library
git clone https://github.com/sisoputnfrba/so-commons-library.git
#	Ingreso a la so-commons-library y la compilo
cd so-commons-library
make
make install
#	Me posiciono en la shared library de nuestro TP y compilo
cd $SHARED
make
#	Me posiciono en la Consola de nuestro TP y compilo
cd $CONSOLACOMPILAR
make
mv Consola $CONSOLA/Consola
#	Me posiciono en el Kernel de nuestro TP y compilo
cd $KERNELCOMPILAR
make
mv Kernel $KERNEL/Kernel
#	Me posiciono en la Memoria de nuestro TP y compilo
cd $MEMORIACOMPILAR
make
mv Memoria $MEMORIA/Memoria
#	Me posiciono en la CPU de nuestro TP y compilo
cd $CPUCOMPILAR
make
mv CPU $CPU/CPU
