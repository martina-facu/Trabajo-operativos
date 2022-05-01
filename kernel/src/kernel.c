#include <stdio.h>
#include <stdlib.h>
#include "./connection/console/server/servidor.h"
#include "./planners/long_term/long_term_planner.h"
int main(void) {
	levantar_servidor_kernel();
	long_term_planner(1, 5, "WRITE");
}
