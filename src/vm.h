#pragma once

#include <stdbool.h>
#include "chunkmem.h"

typedef struct VM {
    size_t ip;
    size_t rb;
    bool halted;
    ChunkMemory mem;
} VM;

VM vm_create();
void vm_destroy(VM* vm);
void vm_load(VM* vm, const char* filename);
void vm_step(VM* vm);
uint64_t vm_runUntilHalt(VM* vm);