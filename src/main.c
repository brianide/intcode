#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "day2.h"

void run(const char* filename) {
    VM vm = vm_create();
    vm_load(&vm, filename);
    vm_runUntilHalt(&vm);
    printf("%lu\n", *mem_getPtr(&vm.mem, 0));
    vm_destroy(&vm);
}

typedef struct Program {
    const char* name;
    void (*func)(const char*);
} Program;

int main(int argc, char** argv) {
    Program progs[] = {
        { "run",   &run   },
        { "day2",  &day2  },
        { "day2b", &day2b }
    };

    for (size_t i = 0; i < sizeof(progs) / sizeof(Program); i++) {
        if (strcmp(progs[i].name, argv[1]) == 0) {
            progs[i].func(argv[2]);
            return 0;
        }
    }

    // TODO Print usage or something
    return 1;
}