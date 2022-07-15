#	Declaro variables de entorno
export TPHOME=$HOME/tp-2022-1c-9-12
export SHARED=$TPHOME/Shared
export CONSOLA=$TPHOME/Consola/compilar
export KERNEL=$TPHOME/Kernel/compilar
export CPU=$TPHOME/CPU/compilar
export MEMORIA=$TPHOME/Memoria/compilar

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
cd $CONSOLA
make
#	Me posiciono en el Kernel de nuestro TP y compilo
cd $KERNEL
make
#	Me posiciono en la CPU de nuestro TP y compilo
cd $CPU
make
#	Me posiciono en la Memoria de nuestro TP y compilo
cd $MEMORIA
make