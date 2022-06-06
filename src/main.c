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
    if (vm_has_output(&vm)) {
        printf("OUT: ");
        for (;;) {
            printf("%ld", vm_get_output(&vm));
            if (!vm_has_output(&vm)) {
                printf("\n");
                break;
            }
            else
                printf(",");
        }
    }
    printf("%ld\n", *mem_get_ptr(&vm.mem, 0));
    vm_destroy(&vm);
}

typedef struct {
    const char* name;
    void (*func)(VMProgram);
    const char* desc;
} RunMode;

static const RunMode MODES[] = {
    { "run",    &run,    "(default) Runs program normally; prints outputs and [0]" },
    { "disas",  &disas,  "Disassembler"                                            },
    { "day2",   &day2,   "Gravity Assist"                                          },
    { "day2b",  &day2b,  "Parameter Modes"                                         },
    { "day5",   &day5,   "T.E.S.T."                                                },
    { "day5b",  &day5b,  "Jumps and Comparisons"                                   },
    { "day7",   &day7,   "Amplification Circuit"                                   },
    { "day7b",  &day7b,  "Feedback Loop"                                           },
    { "day9",   &day9,   "Sensor Boost"                                            },
    { "day9b",  &day9b,  "Feature Completion"                                      },
    { "day11",  &day11,  "Hull Painting Robot"                                     },
    { "day11b", &day11b, "Big Picture"                                             }
};

const RunMode* find_mode(const char* name) {
    for (size_t i = 0; i < sizeof(MODES) / sizeof(RunMode); i++) {
        if (strcmp(MODES[i].name, name) == 0) {
            return &MODES[i];
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
    vm_destroy_program(&prog);

    usage:
    fprintf(stderr, "Usage: %s file [mode]\n\n", argv[0]);
    fprintf(stderr, " MODE  DESCRIPTION\n");
    for (size_t i = 0; i < sizeof(MODES) / sizeof(RunMode); i++) {
        fprintf(stderr, "%5s  %s\n", MODES[i].name, MODES[i].desc);
    }

    return 1;
}