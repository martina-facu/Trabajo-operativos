#include "../estructuras/proceso/proceso.h"
#include "../dev_contants.h"


proceso *NEW[];
proceso *READY[GRADO_MULTIPROGRAMACION];
int READY_ESPACIO_LIBRE = GRADO_MULTIPROGRAMACION;
proceso *EXEC[];
proceso *BLOCKED[];
proceso *SUSPENDED_BLOCKED[];
proceso *SUSPENDED_READY[];
proceso *EXIT[];