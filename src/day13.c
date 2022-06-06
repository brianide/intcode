#include <stdio.h>
#include "days.h"

typedef struct {
    size_t x, y;
} Coord;

typedef enum {
    EMPTY,
    WALL,
    BLOCK,
    HORZ_PADDLE,
    BALL  
} TileType;

#define GRID_SIZE 50

void day13(VMProgram prog) {
    TileType grid[GRID_SIZE][GRID_SIZE] = { EMPTY };
    size_t count = 0;

    VM vm = vm_create();
    vm_load(&vm, &prog);

    for (;;) {
        if (vm_run_til_event(&vm, VM_WAIT_OUTPUT) != VM_RUNNING)
            break;

        Coord coord;
        coord.x = vm_get_output(&vm);
        vm_run_til_event(&vm, VM_WAIT_OUTPUT);
        coord.y = vm_get_output(&vm);
        vm_run_til_event(&vm, VM_WAIT_OUTPUT);
        TileType type = vm_get_output(&vm);

        TileType* tgt = &grid[coord.x][coord.y];
        if (*tgt != BLOCK && type == BLOCK)
            count++;
        else if (*tgt == BLOCK && type != BLOCK)
            count--;
        *tgt = type;
    }

    printf("%zu\n", count);

    vm_destroy(&vm);
}

void day13b(VMProgram prog) {
}