#pragma once

#include <stdbool.h>
#include "chunkmem.h"
#include "linkedqueue.h"

typedef struct {
    size_t ip;
    size_t rb;
    bool halted;
    ChunkMemory mem;
    Queue input;
    Queue output;
} VM;

VM vm_create();
void vm_destroy(VM* vm);
void vm_appendInput(VM* vm, int64_t value);
bool vm_tryGetOutput(VM* vm, int64_t* out);
void vm_load(VM* vm, const char* filename);
void vm_step(VM* vm);
uint64_t vm_runUntilHalt(VM* vm);