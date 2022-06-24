#	Cargo la LD_LIBRARY_PATH de manera que no de error de compilacion
export LD_LIBRARY_PATH=/home/utnso/tp-2022-1c-9-12/Shared/Debug

#	Borro el contenido del archivo de log del valgrind para la CPU
> /home/utnso/tp-2022-1c-9-12/valgrind/cpu_valgrind.log

#	Ingreso al directorio de ejecucion de la CPU para poder ejecutar el valgrind sin PATH absoluto.
cd /home/utnso/tp-2022-1c-9-12/CPU/Debug

#	Copio el archivo de configuracion de la CPU para mantenerlo actualizado
cp /home/utnso/tp-2022-1c-9-12/CPU/cpu.config ./

#	Ejecuto el proceso con valgrind con todos los parametros que fui obteniendo de sucesivas ejecuciones
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=/home/utnso/tp-2022-1c-9-12/valgrind/cpu_valgrind.log -v CPU

#	Luego del valgrind ejecuto un more del archivo de log para poder hacer la revision de la ejecucion.
more /home/utnso/tp-2022-1c-9-12/valgrind/cpu_valgrind.log
