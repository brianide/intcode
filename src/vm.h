#pragma once

#include <stdbool.h>
#include "chunkmem.h"
#include "linkedqueue.h"

typedef enum {
    VM_RUNNING,
    VM_HALTED,
    VM_ERROR
} VMState;

typedef struct {
    size_t ip;
    size_t rb;
    VMState state;
    ChunkMemory mem;
    Queue input;
    Queue output;
} VM;

typedef struct {
    size_t length;
    int64_t* data;
} VMProgram;

// Lifecycle
VM vm_create();
void vm_destroy(VM* vm);

// Input/Output
void vm_append_input(VM* vm, int64_t value);
bool vm_try_get_output(VM* vm, int64_t* out);
size_t vm_has_output(VM* vm);
int64_t vm_get_output(VM* vm);

// Program Loading
VMProgram vm_parse_program(const char* path);
void vm_destroy_program(VMProgram* prog);
void vm_load(VM* vm, VMProgram* program);
void vm_load_file(VM* vm, const char* file);

// Program Execution
void vm_step(VM* vm);
uint64_t vm_run_til_halt(VM* vm);
VMState vm_run_til_event(VM* vm);