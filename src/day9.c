#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "days.h"

static void runWith(VMProgram prog, int64_t input) {
    VM vm = vm_create();
    vm_load(&vm, &prog);
    vm_append_input(&vm, input);
    vm_run_til_event(&vm, VM_WAIT_OUTPUT);
    printf("%ld\n", vm_get_output(&vm));
    vm_destroy(&vm);
}

void day9(VMProgram prog) {
    runWith(prog, 1);
}

void day9b(VMProgram prog) {
    runWith(prog, 2);
}