#pragma once

#include <stdbool.h>
#include "chunkmem.h"

#define VM_QUEUE_MAX 32

#define VM_WAIT_BLOCK_INPUT 0b001
#define VM_WAIT_ANY_OUTPUT  0b010
#define VM_WAIT_FULL_OUTPUT 0b100

typedef enum {
    VM_RUNNING,
    VM_HALTED,
    VM_ERROR,
    VM_QUEUE_OVERFLOW,
    VM_INVALID_MODE,
    VM_INVALID_INSTRUCTION
} VMState;

typedef struct {
    uint_fast8_t write;
    uint_fast8_t read;
    uint_fast8_t available;
    int64_t buffer[VM_QUEUE_MAX];
} IntQueue;

typedef struct {
    size_t ip;
    size_t rb;
    VMState state;
    ChunkMemory mem;
    IntQueue input;
    IntQueue output;
} VM;

typedef struct {
    size_t length;
    int64_t data[];
} VMProgram;

// Lifecycle
VM* vm_create();
void vm_destroy(VM* vm);

// Input/Output
bool vm_push_input(VM* vm, int64_t value);
bool vm_try_get_output(VM* vm, int64_t* out);
size_t vm_has_output(VM* vm);
int64_t vm_get_output(VM* vm);
bool vm_output_full(VM* vm);

// Program Loading
VMProgram* vm_parse_program(const char* path);
void vm_destroy_program(VMProgram* prog);
void vm_load(VM* vm, VMProgram* program);
void vm_load_file(VM* vm, const char* file);

// Program Execution
void vm_step(VM* vm);
uint64_t vm_run_til_halt(VM* vm);
VMState vm_run_til_event(VM* vm, uint8_t flags);
bool vm_awaiting_input(VM* vm);

// Misc
void vm_dump_state(VM* vm);