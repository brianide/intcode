#include <stdio.h>
#include <raylib.h>
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

#define GRID_WIDTH 40
#define GRID_HEIGHT 21

void day13(VMProgram* prog) {
    TileType grid[GRID_HEIGHT][GRID_WIDTH] = { EMPTY };
    size_t count = 0;

    VM* vm = vm_create();
    vm_load(vm, prog);

    for (;;) {
        if (vm_run_til_event(vm, VM_WAIT_OUTPUT) != VM_RUNNING)
            break;

        Coord coord;
        coord.x = vm_get_output(vm);
        vm_run_til_event(vm, VM_WAIT_OUTPUT);
        coord.y = vm_get_output(vm);
        vm_run_til_event(vm, VM_WAIT_OUTPUT);
        TileType type = vm_get_output(vm);

        TileType* tgt = &grid[coord.x][coord.y];
        if (*tgt != BLOCK && type == BLOCK)
            count++;
        else if (*tgt == BLOCK && type != BLOCK)
            count--;
        *tgt = type;
    }

    printf("%zu\n", count);

    vm_destroy(vm);
}

#define BLOCK_SIZE 6
#define DRAW_OFFSET 30

Color get_color_for(TileType tile) {
    switch (tile) {
        case WALL: return BROWN;
        case BLOCK: return RED;
        case HORZ_PADDLE: return BLUE;
        case BALL: return BLACK;
        default: return RAYWHITE;
    }
}

void day13b(VMProgram* prog) {
    TileType grid[GRID_HEIGHT][GRID_WIDTH] = { EMPTY };
    int64_t score = 0;

    // Load program and tweak address
    VM* vm = vm_create();
    vm_load(vm, prog);
    *mem_get_ptr(&vm->mem, 0) = 2;

    // Pointers, for cheating
    int64_t* ballx = mem_get_ptr(&vm->mem, 388);
    int64_t* paddlex = mem_get_ptr(&vm->mem, 392);
    
    // Setup for drawing
    const int width = GRID_WIDTH * BLOCK_SIZE;
    const int height = GRID_HEIGHT * BLOCK_SIZE + DRAW_OFFSET;
    SetTraceLogLevel(LOG_FATAL);
    InitWindow(width, height, "intcode");
    SetTargetFPS(120);

    while (!WindowShouldClose()) {

        if (vm_run_til_event(vm, VM_WAIT_OUTPUT | VM_WAIT_INPUT) != VM_RUNNING)
            break;

        // If an input is available, take it and also grab the next two
        if (vm_has_output(vm)) {
            const int64_t x = vm_get_output(vm);
            vm_run_til_event(vm, VM_WAIT_OUTPUT);
            const int64_t y = vm_get_output(vm);
            vm_run_til_event(vm, VM_WAIT_OUTPUT);

            // Update display
            if (x == -1 && y == 0)
                score = vm_get_output(vm);
            else
                grid[y][x] = vm_get_output(vm);
        }

        // Don't draw until an input frame occurs
        if (!vm_awaiting_input(vm))
            continue;

        // Cheat
        const int64_t input = *ballx > *paddlex ? 1 : *ballx < *paddlex ? -1 : 0;
        vm_push_input(vm, input);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw score and debug info
        DrawText(TextFormat("Score: %ld", score), 3, 3, 10, RED);
        DrawText(TextFormat("%ld %ld", *ballx, *paddlex), 3, 13, 10, RED);

        // Draw grid cells
        for (size_t y = 0; y < GRID_HEIGHT; y++) {
            for (size_t x = 0; x < GRID_WIDTH; x++) {
                TileType tile = grid[y][x];
                if (tile != EMPTY) {
                    DrawRectangle(x * (BLOCK_SIZE), DRAW_OFFSET + y * (BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE, get_color_for(tile));
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();
    printf("%zu\n", score);
    vm_destroy(vm);
}