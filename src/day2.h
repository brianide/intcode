#pragma once

#include <stdio.h>
#include "vm.h"

void day2(const char* filename) {
    VM vm = vm_create();
    vm_load(&vm, filename);

    // Replace prescribed values
    vm.mem.cursor = 1;
    mem_append(&vm.mem, 12);
    mem_append(&vm.mem, 2);

    vm_runUntilHalt(&vm);
    printf("%lu\n", *mem_getPtr(&vm.mem, 0));
    vm_destroy(&vm);
}

void day2b(const char* filename) {
    int64_t noun;
    int64_t verb;
    int64_t result;

    for (noun = 0; noun <= 99; noun++) {
        for (verb = 0; verb <= 99; verb++) {
            VM vm = vm_create();
            vm_load(&vm, filename);

            vm.mem.cursor = 1;
            mem_append(&vm.mem, noun);
            mem_append(&vm.mem, verb);

            vm_runUntilHalt(&vm);
            int64_t result = *mem_getPtr(&vm.mem, 0);
            vm_destroy(&vm);

            if (result == 19690720) {
                goto end;
            }
        }
    }

    end: printf("%lu\n", 100 * noun + verb);
}