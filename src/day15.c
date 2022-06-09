#include <stdio.h>
#include <raylib.h>
#include "modes.h"
#include "arraylist.h"

typedef enum {
    NORTH = 1,
    SOUTH,
    WEST,
    EAST
} Direction;

typedef enum {
    WALL,
    FLOOR,
    SYSTEM
} CellState;

typedef struct {
    int64_t x, y;
    CellState state;
} Cell;

typedef struct {
    int32_t x, y;
} Coord;

typedef struct {
    Coord pos;
    Direction heading;
} BotLoc;

static void record_cell(ArrayList** list, int64_t x, int64_t y, CellState state) {
    for (Cell* c = alist_begin(*list); c != alist_end(*list); c++) {
        if (c->x == x && c->y == y)
            return;
    }
    *(Cell*) alist_new_ptr(list) = (Cell) { x, y, state };
}

#define GRID_CELL_SIZE 10
#define GRID_GUTTER 4

static inline Direction turn(BotLoc* p) {
    switch (p->heading) {
        case NORTH: p->heading = EAST; break;
        case EAST: p->heading = SOUTH; break;
        case SOUTH: p->heading = WEST; break;
        case WEST: p->heading = NORTH; break;
    }
    return p->heading;
}

static const char* directionName(Direction dir) {
    switch (dir) {
        case NORTH: return "NORTH";
        case EAST: return "EAST";
        case SOUTH: return "SOUTH";
        case WEST: return "WEST";
    }
}

static void doManualInput(VM* vm, BotLoc* botpos, ArrayList** visited) {
    for (;;) {
        Direction bearing;
        Coord target;

        switch (GetKeyPressed()) {
            case 0:
            default:
                goto end_input;
                
            case KEY_UP:
                bearing = NORTH;
                target = (Coord) { botpos->pos.x, botpos->pos.y - 1 };
                break;
            case KEY_DOWN:
                bearing = SOUTH;
                target = (Coord) { botpos->pos.x, botpos->pos.y + 1 };
                break;
            case KEY_LEFT:
                bearing = WEST;
                target = (Coord) { botpos->pos.x - 1, botpos->pos.y };
                break;
            case KEY_RIGHT:
                bearing = EAST;
                target = (Coord) { botpos->pos.x + 1, botpos->pos.y };
                break;
        }
        vm_push_input(vm, bearing);
        vm_run_til_event(vm, VM_WAIT_ANY_OUTPUT);
        CellState resp = vm_get_output(vm);
        record_cell(visited, target.x, target.y, resp);

        if (resp != WALL) {
            botpos->heading = bearing;
            botpos->pos = target;
        }
    }
    end_input:;
}

void day15(VMProgram* prog) {
    VM* vm = vm_create();
    vm_load(vm, prog);

    BotLoc botpos = { {0, 0}, WEST };
    ArrayList* visited = alist_create(100, sizeof(Cell));
    record_cell(&visited, botpos.pos.x, botpos.pos.y, FLOOR);

    SetTraceLogLevel(LOG_NONE);
    SetTargetFPS(30);
    InitWindow(500,300, "intcode");
    Camera2D cam = {
        .zoom = 1.0,
        .offset = (Vector2) { 250, 150 },
        .rotation = 0,
        .target = (Vector2) { 0, 0 }
    };

    while (!WindowShouldClose()) {
        // Run VM until input is requested
        vm_run_til_event(vm, VM_WAIT_BLOCK_INPUT);
        if (vm->state != VM_RUNNING)
            break;
        
        // Process all queued inputs
        if (vm_awaiting_input(vm)) {
            doManualInput(vm, &botpos, &visited);
        }

        cam.target = (Vector2) { botpos.pos.x * GRID_CELL_SIZE, botpos.pos.y * GRID_CELL_SIZE };

        BeginDrawing();
        BeginMode2D(cam);
        ClearBackground(BLACK);
        for (Cell* c = alist_begin(visited); c != alist_end(visited); c++) {
            Color color =
                c->state == WALL ? BROWN
                : c->state == FLOOR ? GRAY
                : c-> state == SYSTEM ? GREEN
                : PURPLE;
            DrawRectangle(c->x * GRID_CELL_SIZE, c->y * GRID_CELL_SIZE, GRID_CELL_SIZE, GRID_CELL_SIZE, color);
        }
        DrawCircle(botpos.pos.x * GRID_CELL_SIZE + GRID_CELL_SIZE / 2, botpos.pos.y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2, RED);
        EndMode2D();

        DrawText(directionName(botpos.heading), 4, 4, 10, RED);
        EndDrawing();
    }

    vm_destroy(vm);
}

void day15b(VMProgram* prog) {
}