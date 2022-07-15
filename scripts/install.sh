#	Declaro variables de entorno
export TPHOME=$HOME/tp-2022-1c-9-12
export SHARED=$TPHOME/Shared
export CONSOLA=$TPHOME/Consola/src
export KERNEL=$TPHOME/Kernel/src
export CPU=$TPHOME/CPU/src
export MEMORIA=$TPHOME/Memoria/src

#	Me posiciono sobre el home del usuario
cd
#	Clono la so-commons-library
git clone https://github.com/sisoputnfrba/so-commons-library.git
#	Ingreso a la so-commons-library y la compilo
cd so-commons-library
make
make install
#	Me posiciono en la shared library de nuestro TP
cd
cd $SHARED
make
make install
