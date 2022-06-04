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

typedef struct {
    size_t length;
    int64_t* data;
} VMProgram;

VM vm_create();
void vm_destroy(VM* vm);
void vm_appendInput(VM* vm, int64_t value);
bool vm_tryGetOutput(VM* vm, int64_t* out);
VMProgram vm_parse_program(const char* path);
void vm_destroy_program(VMProgram* prog);
void vm_load(VM* vm, VMProgram* program);
void vm_load_file(VM* vm, const char* file);
void vm_step(VM* vm);
uint64_t vm_runUntilHalt(VM* vm);