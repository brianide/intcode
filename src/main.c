#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "days.h"

#include "linkedqueue.h"

void run(const char* filename) {
    VM vm = vm_create();
    vm_load_file(&vm, filename);
    vm_runUntilHalt(&vm);
    printf("%lu\n", *mem_getPtr(&vm.mem, 0));
    vm_destroy(&vm);
}

typedef struct Program {
    const char* name;
    void (*func)(const char*);
    const char* desc;
} Program;

static const Program progs[] = {
    { "run",   &run ,  "(default) Runs program normally; prints value at address 0" },
    { "day2",  &day2,  "Gravity Assist" },
    { "day2b", &day2b, "Parameter Modes" },
    { "day5",  &day5,  "TEST" },
    { "day5b", &day5b, "Jumps and Comparisons" },
    { "day7",  &day7,  "Amplification Circuit" }
};

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3)
        goto usage;

    const char* file = argv[1];

    if (argc == 2) {
        run(file);
        return 0;
    }

    for (size_t i = 0; i < sizeof(progs) / sizeof(Program); i++) {
        if (strcmp(progs[i].name, argv[2]) == 0) {
            progs[i].func(file);
            return 0;
        }
    }

    usage:
    fprintf(stderr, "Usage: %s file [mode]\n\n", argv[0]);
    fprintf(stderr, " MODE  DESCRIPTION\n");
    for (size_t i = 0; i < sizeof(progs) / sizeof(Program); i++) {
        fprintf(stderr, "%5s  %s\n", progs[i].name, progs[i].desc);
    }

    return 1;
}