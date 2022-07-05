#set -x
ps -ef | grep Memoria | grep -v grep | awk '{print $2}' | xargs kill -9
ps -ef | grep Kernel | grep -v grep | awk '{print $2}' | xargs kill -9
ps -ef | grep CPU | grep -v grep | awk '{print $2}' | xargs kill -9
ps -ef | grep consola | grep -v grep | awk '{print $2}' | xargs kill -9
