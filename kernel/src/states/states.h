#include "../domain/process/process.h"
#include "../dev_contants.h"


process *NEW[];
process *READY[MULTIPROCESSING_LEVEL];
int READY_FREE_SPACE = MULTIPROCESSING_LEVEL;
process *EXEC[];
process *BLOCKED[];
process *SUSPENDED_BLOCKED[];
process *SUSPENDED_READY[];
process *EXIT[];

