# Faltantes:
	Validar Clock-M - []
	Activar - Probar TLB - []
	Activar COPY en CPU - []
	Validar escritura del Swap - []

# Errores
	Ver Issues

# Scripts:
($WORKSPACE/scripts)
- reiniciarLog.sh:	Reinicia todos los logs.
- reiniciarEclipseEnv.sh:	Reinicia el ambiente de Eclipse luego de haber realizado un clone. Para eso copia le copia los config del base a cada uno de los proyectos.
- psTP.sh:	Muestra todos los procesos levantados de los proyectos.
- kpsTP.sh:	Mata todos los procesos levantados de los proyectos.
- install.sh:	Instala todos los componentes necesarios para el TP y compila los distintos proyectos del mismo.
- rollback.sh	Desinstala todos los componentes para poder volver a instalar el TP. Luego de esto se debera hacer un rm -R $WORKSPACE (reemplazar por el directorio correspondiente) para liberar la carpeta donde se clono.
- basicExec.sh:	Ejecuta la prueba basica completa.
- integralExec.sh: Ejecuta la prueba integral completa.
- mclockExec.sh: Ejecuta la prueba con algoritmo Clock en Memoria completa.
- mclockmExec.sh: Ejecuta la prueba con algoritmo Clock-M en Memoria  completa.
- mtlbFIFOExec.sh: Ejecuta la prueba con algoritmo FIFO en TLB completa.
- mtlbLRUExec.sh:	Ejecuta la prueba con algoritmo LRU en TLB  completa.
- planiFIFOExec.sh: Ejecuta la prueba con algoritmo FIFO en planificadores de Kernel completa.
- planiSRTExec.sh: Ejecuta la prueba con algoritmo SRT en planificadores de Kernel completa.
- suspFIFOExec.sh: Ejecuta la prueba con algoritmo FIFO en planificadores de Kernel con SUSPENCION completa.
- suspSRTExec.sh: Ejecuta la prueba con algoritmo SRT en planificadores de Kernel con SUSPENCION completa.
