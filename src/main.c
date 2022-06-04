#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "days.h"

#include "linkedqueue.h"

void run(VMProgram prog) {
    VM vm = vm_create();
    vm_load(&vm, &prog);
    vm_run_til_halt(&vm);
    printf("%lu\n", *mem_get_ptr(&vm.mem, 0));
    vm_destroy(&vm);
}

typedef struct RunMode {
    const char* name;
    void (*func)(VMProgram);
    const char* desc;
} RunMode;

static const RunMode modes[] = {
    { "run",   &run ,  "(default) Runs program normally; prints value at address 0" },
    { "day2",  &day2,  "Gravity Assist" },
    { "day2b", &day2b, "Parameter Modes" },
    { "day5",  &day5,  "T.E.S.T." },
    { "day5b", &day5b, "Jumps and Comparisons" },
    { "day7",  &day7,  "Amplification Circuit" },
    { "day7b", &day7b, "Feedback Loop" }
};

const RunMode* find_mode(const char* name) {
    for (size_t i = 0; i < sizeof(modes) / sizeof(RunMode); i++) {
        if (strcmp(modes[i].name, name) == 0) {
            return &modes[i];
        }
    }

    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3)
        goto usage;

    VMProgram prog = vm_parse_program(argv[1]);

    if (argc == 2) {
        run(prog);
        vm_destroy_program(&prog);
        return 0;
    }
    else {
        const RunMode* mode = find_mode(argv[2]);
        if (mode) {
            mode->func(prog);
            vm_destroy_program(&prog);
            return 0;
        }
    }

    usage:
    fprintf(stderr, "Usage: %s file [mode]\n\n", argv[0]);
    fprintf(stderr, " MODE  DESCRIPTION\n");
    for (size_t i = 0; i < sizeof(modes) / sizeof(RunMode); i++) {
        fprintf(stderr, "%5s  %s\n", modes[i].name, modes[i].desc);
    }
    return 1;
}