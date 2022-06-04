#include <stdio.h>
#include "vm.h"
#include "days.h"

static int64_t runWith(VMProgram* prog, int64_t noun, int64_t verb) {
    VM vm = vm_create();
    vm_load(&vm, prog);

    vm.mem.cursor = 1;
    mem_append(&vm.mem, noun);
    mem_append(&vm.mem, verb);

    vm_runUntilHalt(&vm);
    int64_t result = *mem_getPtr(&vm.mem, 0);
    vm_destroy(&vm);

    return result;
}

void day2(const char* filename) {
    VMProgram prog = vm_parse_program(filename);
    printf("%lu\n", runWith(&prog, 12, 2));
    vm_destroy_program(&prog);
}

void day2b(const char* filename) {
    int64_t noun;
    int64_t verb;
    int64_t result;

    VMProgram prog = vm_parse_program(filename);

    for (noun = 0; noun <= 99; noun++) {
        for (verb = 0; verb <= 99; verb++) {
            if (runWith(&prog, noun, verb) == 19690720) {
                goto end;
            }
        }
    }
    end:
    
    vm_destroy_program(&prog);
    printf("%lu\n", 100 * noun + verb);
}