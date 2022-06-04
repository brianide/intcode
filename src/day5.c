#include <stdio.h>
#include "vm.h"
#include "days.h"

void day5(const char* filename) {
    VM vm = vm_create();
    vm_load_file(&vm, filename);
    vm_appendInput(&vm, 1);
    vm_runUntilHalt(&vm);
    
    int64_t diag = 0;
    while (diag == 0)
        vm_tryGetOutput(&vm, &diag);

    printf("%ld\n", diag);

    vm_destroy(&vm);
}

void day5b(const char* filename) {
    VM vm = vm_create();
    vm_load_file(&vm, filename);
    vm_appendInput(&vm, 5);
    vm_runUntilHalt(&vm);
    
    int64_t diag;
    vm_tryGetOutput(&vm, &diag);    
    printf("%ld\n", diag);

    vm_destroy(&vm);
}