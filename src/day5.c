#include <stdio.h>
#include "days.h"

void day5(VMProgram prog) {
    VM vm = vm_create();
    vm_load(&vm, &prog);
    vm_append_input(&vm, 1);
    vm_run_til_halt(&vm);
    
    int64_t diag = 0;
    while (diag == 0)
        vm_try_get_output(&vm, &diag);

    printf("%ld\n", diag);

    vm_destroy(&vm);
}

void day5b(VMProgram prog) {
    VM vm = vm_create();
    vm_load(&vm, &prog);
    vm_append_input(&vm, 5);
    vm_run_til_halt(&vm);
    
    int64_t diag;
    vm_try_get_output(&vm, &diag);    
    printf("%ld\n", diag);

    vm_destroy(&vm);
}