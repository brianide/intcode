#include <stdio.h>
#include "days.h"

static int64_t runWith(VMProgram* prog, int64_t noun, int64_t verb) {
    VM vm = vm_create();
    vm_load(&vm, prog);

    *mem_get_ptr(&vm.mem, 1) = noun;
    *mem_get_ptr(&vm.mem, 2) = verb;

    vm_run_til_halt(&vm);
    int64_t result = *mem_get_ptr(&vm.mem, 0);
    vm_destroy(&vm);

    return result;
}

void day2(VMProgram prog) {
    printf("%lu\n", runWith(&prog, 12, 2));
}

void day2b(VMProgram prog) {
    int64_t noun;
    int64_t verb;
    int64_t result;

    for (noun = 0; noun <= 99; noun++) {
        for (verb = 0; verb <= 99; verb++) {
            if (runWith(&prog, noun, verb) == 19690720) {
                goto end;
            }
        }
    }
    end:
    
    printf("%lu\n", 100 * noun + verb);
}