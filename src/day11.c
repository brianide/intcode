#include <stdio.h>
#include "modes.h"

typedef enum {
    UNPAINTED,
    BLACK,
    WHITE
} PanelState;

typedef enum {
    DOWN,
    RIGHT,
    UP,
    LEFT
} Direction;

typedef enum {
    MOVE,
    PAINT
} NextAction;

typedef struct {
    uint16_t x, y;
    Direction bearing;
    NextAction next_act;
} BotState;

#define GRID_SIZE 1000

// TODO Currently using a strategy where we pause execution whenever an INP
// instruction is in the chamber and then throw a value into the queue before
// resuming. Revisit this later and replace this goofy shit with a proper
// polymorphic solution.
static uint64_t runWith(PanelState hull[GRID_SIZE][GRID_SIZE], VM* vm) {
    BotState bot = { .x = GRID_SIZE / 2, .y = GRID_SIZE / 2, .bearing = UP, .next_act = PAINT };
    uint64_t painted = 0;

    for (;;) {
        if (vm_run_til_event(vm, VM_WAIT_BLOCK_INPUT | VM_WAIT_ANY_OUTPUT) == VM_HALTED) {
            break;
        }
    
        if (vm_has_output(vm)) {
            if (bot.next_act == MOVE) {
                bot.bearing = (bot.bearing + (1 - 2 * vm_get_output(vm) + 4)) % 4;

                switch (bot.bearing) {
                    case UP: bot.y--; break;
                    case DOWN: bot.y++; break;
                    case LEFT: bot.x--; break;
                    case RIGHT: bot.x++; break;
                }
                bot.next_act = PAINT;
            }
            else {
                if (hull[bot.x][bot.y] == UNPAINTED)
                    painted++;
                hull[bot.x][bot.y] = vm_get_output(vm) + 1;
                bot.next_act = MOVE;
            }
        }

        if (vm_awaiting_input(vm)) {
            vm_push_input(vm, hull[bot.x][bot.y] == WHITE ? 1 : 0);
        }
    }

    return painted;
}

void day11(VMProgram* prog) {
    VM* vm = vm_create();
    vm_load(vm, prog);
    PanelState hull[GRID_SIZE][GRID_SIZE] = { UNPAINTED };

    uint64_t painted = runWith(hull, vm);
    printf("%lu\n", painted);
    
    vm_destroy(vm);
}

void day11b(VMProgram* prog) {
    VM* vm = vm_create();
    vm_load(vm, prog);
    PanelState hull[GRID_SIZE][GRID_SIZE] = { UNPAINTED };
    hull[GRID_SIZE / 2][GRID_SIZE / 2] = WHITE;

    runWith(hull, vm);

    size_t minx = SIZE_MAX, maxx = 0, miny = SIZE_MAX, maxy = 0;

    for (size_t y = 0; y < GRID_SIZE; y++) {
        for (size_t x = 0; x < GRID_SIZE; x++) {
            if (hull[x][y] == WHITE) {
                if (x < minx) minx = x;
                if (y < miny) miny = y;
                if (x > maxx) maxx = x;
                if (y > maxy) maxy = y;
            }
        }
    }

    for (size_t y = miny; y <= maxy; y++) {
        for (size_t x = minx; x <= maxx; x++) {
            printf("%s", hull[x][y] == WHITE ? "#" : " ");
        }
        printf("\n");
    }

    vm_destroy(vm);
}