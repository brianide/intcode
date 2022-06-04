#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "days.h"

void day9(VMProgram prog) {
    VM vm = vm_create();
    vm_load(&vm, &prog);
    vm_run_til_event(&vm);
    printf("%ld\n", vm_get_output(&vm));
    vm_destroy(&vm);
}

void day9b(VMProgram prog) {
}