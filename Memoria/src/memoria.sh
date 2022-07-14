#	Cargo la LD_LIBRARY_PATH de manera que no de error de compilacion
export LD_LIBRARY_PATH=/home/utnso/tp-2022-1c-9-12/Shared/Debug

#	Borro el contenido del archivo de log del valgrind para la MEMORIA
> /home/utnso/tp-2022-1c-9-12/valgrind/memoria_valgrind.log

#	Ingreso al directorio de ejecucion de la MEMORIA para poder ejecutar el valgrind sin PATH absoluto.
cd /home/utnso/tp-2022-1c-9-12/Memoria/Debug

#	Copio el archivo de configuracion de la MEMORIA para mantenerlo actualizado
cp /home/utnso/tp-2022-1c-9-12/Memoria/memoria.config ./

#	Ejecuto el proceso con valgrind con todos los parametros que fui obteniendo de sucesivas ejecuciones
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=/home/utnso/tp-2022-1c-9-12/valgrind/memoria_valgrind.log -v Memoria
valgrind --show-leak-kinds=all --leak-check=full --track-origins=yes --log-file=/home/utnso/tp-2022-1c-9-12/valgrind/memoria_valgrind.log -v Memoria

#	Luego del valgrind ejecuto un more del archivo de log para poder hacer la revision de la ejecucion.
more /home/utnso/tp-2022-1c-9-12/valgrind/memoria_valgrind.log
