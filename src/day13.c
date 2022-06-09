#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include "modes.h"

typedef enum {
    EMPTY,
    WALL,
    BLOCK,
    HORZ_PADDLE,
    BALL  
} TileType;

#define BLOCK_SIZE 6
#define DRAW_OFFSET 40

#define INPUTS_PER_FRAME 2

#define ADDR_BALL_X 388
#define ADDR_BALL_VEL 390
#define ADDR_PADDLE_X 392

typedef struct {
    int64_t score;
    uint64_t block_count;
} RunResult;

static Color get_color_for(TileType tile) {
    switch (tile) {
        case WALL: return BROWN;
        case BLOCK: return RED;
        case HORZ_PADDLE: return BLUE;
        case BALL: return BLACK;
        default: return RAYWHITE;
    }
}

static RunResult run(VMProgram* prog, const size_t grid_width, const size_t grid_height, bool render) {
    TileType grid[grid_height][grid_width];
    memset(grid, 0, grid_width * grid_height * sizeof(TileType));

    int64_t score = 0;
    uint64_t count = 0;
    uint64_t frame_counter = 0;

    // Load program and tweak address
    VM* vm = vm_create();
    vm_load(vm, prog);
    
    // Setup for drawing
    if (render) {
        const int width = grid_width * BLOCK_SIZE;
        const int height = grid_height * BLOCK_SIZE + DRAW_OFFSET;
        SetTraceLogLevel(LOG_FATAL);
        InitWindow(width, height, "intcode");
        SetTargetFPS(60);
    }

    while (!render && vm->state == VM_RUNNING || !WindowShouldClose()) {

        vm_run_til_event(vm, VM_WAIT_BLOCK_INPUT | VM_WAIT_FULL_OUTPUT);

        // If an input is available, take it and also grab the next two
        while (vm_has_output(vm) >= 3) {
            const int64_t x = vm_get_output(vm);
            const int64_t y = vm_get_output(vm);

            // Update display
            if (x == -1 && y == 0)
                score = vm_get_output(vm);
            else {
                TileType type = vm_get_output(vm);
                TileType* tgt = &grid[y][x];

                if (*tgt != BLOCK && type == BLOCK)
                    count++;
                else if (*tgt == BLOCK && type != BLOCK)
                    count--;
                *tgt = type;
            }
        }

        // Keep running until the VM requests an input
        if (vm->state == VM_RUNNING && !vm_awaiting_input(vm))
            continue;

        // Cheat
        const int64_t ballx = *mem_get_ptr(&vm->mem, ADDR_BALL_X);
        const int64_t ballvel = *mem_get_ptr(&vm->mem, ADDR_BALL_VEL);
        const int64_t paddlex = *mem_get_ptr(&vm->mem, ADDR_PADDLE_X);

        const int64_t input = ballx > paddlex ? 1 : ballx < paddlex ? -1 : 0;
        vm_push_input(vm, input);

        // Limit how often we render relative to how frequently input is polled
        if (!render || frame_counter++ % INPUTS_PER_FRAME)
            continue;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw score and debug info
        DrawText(TextFormat("Score: %ld", score), 3, 3, 10, RED);
        DrawText(TextFormat("%ld %ld %ld", ballx, ballvel, paddlex), 3, 13, 10, RED);

        // Draw grid cells
        for (size_t y = 0; y < grid_height; y++) {
            for (size_t x = 0; x < grid_width; x++) {
                TileType tile = grid[y][x];
                if (tile != EMPTY) {
                    DrawRectangle(x * (BLOCK_SIZE), DRAW_OFFSET + y * (BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE, get_color_for(tile));
                }
            }
        }

        // Draw state if the VM has stopped
        if (vm->state != VM_RUNNING) {
            DrawText(vm_state_name(vm->state), 3, 23, 10, RED);
        }

        EndDrawing();
    }

    vm_destroy(vm);

    if (render)
        CloseWindow();

    return (RunResult) {
        .score = score,
        .block_count = count
    };
}

void day13(VMProgram* prog) {
    RunResult res = run(prog, 50, 50, false);
    printf("%zu\n", res.block_count);
}

void day13b(VMProgram* prog) {
    prog->data[0] = 2;
    RunResult res = run(prog, 40, 21, false);
    printf("%ld\n", res.score);
}

void day13v(VMProgram* prog) {
    prog->data[0] = 2;
    RunResult res = run(prog, 40, 21, true);
    printf("%ld\n", res.score);
}