#include <stdio.h>
#include "vm.h"
#include "days.h"

static int64_t runWith(const char* filename, int64_t noun, int64_t verb) {
    VM vm = vm_create();
    vm_load(&vm, filename);

    vm.mem.cursor = 1;
    mem_append(&vm.mem, noun);
    mem_append(&vm.mem, verb);

    vm_runUntilHalt(&vm);
    int64_t result = *mem_getPtr(&vm.mem, 0);
    vm_destroy(&vm);

    return result;
}

void day2(const char* filename) {
    printf("%lu\n", runWith(filename, 12, 2));
}

void day2b(const char* filename) {
    int64_t noun;
    int64_t verb;
    int64_t result;

    for (noun = 0; noun <= 99; noun++) {
        for (verb = 0; verb <= 99; verb++) {
            if (runWith(filename, noun, verb) == 19690720) {
                goto end;
            }
        }
    }

    end: printf("%lu\n", 100 * noun + verb);
}