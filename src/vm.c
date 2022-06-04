#include <stdio.h>
#include "vm.h"

#define MODE_POS 0
#define MODE_IMM 1
#define MODE_REL 2

#define OP_ADD 1
#define OP_MUL 2
#define OP_INP 3
#define OP_OUT 4
#define OP_JNZ 5
#define OP_JEZ 6
#define OP_TLT 7
#define OP_TEQ 8
#define OP_ARB 9
#define OP_HLT 99

// LIFECYCLE //////////////////////////////////////////////////////////////////

VM vm_create() {
    return (VM) {
        .ip = 0,
        .rb = 0,
        .state = VM_RUNNING,
        .mem = mem_create()
    };
}

void vm_destroy(VM* vm) {
    mem_destroy(&vm->mem);
    queue_destroy(&vm->input, &free);
    queue_destroy(&vm->output, &free);
}

// INPUT/OUTPUT ///////////////////////////////////////////////////////////////

void vm_append_input(VM* vm, int64_t value) {
    //queue_push(&vm->input, wrap(value));
    queue_push_value(&vm->input, value);
}

bool vm_try_get_output(VM* vm, int64_t* out) {
    int64_t* result;
    if (queue_try_remove(&vm->output, (void**) &result)) {
        *out = *result;
        free(result);
        return true;
    }
    return false;
}

size_t vm_has_output(VM* vm) {
    return vm->output.length;
}

int64_t vm_get_output(VM* vm) {
    return queue_remove_value(&vm->output, int64_t);
}

// PROGRAM LOADING ////////////////////////////////////////////////////////////

VMProgram vm_parse_program(const char* path) {
    size_t capacity = 1024;
    int64_t* buffer = calloc(capacity, sizeof(int64_t));
    size_t cursor = 0;

    FILE* f = fopen(path, "r");
    int64_t value;
    while (!feof(f) && fscanf(f, "%ld,", &value) == 1) {
        if (cursor == capacity) {
            capacity *= 1.5;
            buffer = realloc(buffer, capacity * sizeof(int64_t));
        }
        buffer[cursor++] = value;
    }
    fclose(f);

    buffer = realloc(buffer, cursor * sizeof(int64_t));
    return (VMProgram) { .length = cursor, .data = buffer };
}

void vm_destroy_program(VMProgram* prog) {
    free(prog->data);
}

void vm_load(VM* vm, VMProgram* prog) {
    for (size_t i = 0; i < prog->length; i++)
        *mem_get_ptr(&vm->mem, i) = prog->data[i];
}

void vm_load_file(VM* vm, const char* file) {
    VMProgram prog = vm_parse_program(file);
    vm_load(vm, &prog);
    vm_destroy_program(&prog);
}

// PROGRAM EXECUTION //////////////////////////////////////////////////////////

static int64_t* param(VM* vm, size_t pos) {
    int64_t mode = *mem_get_ptr(&vm->mem, vm->ip) / 100;
    for (size_t i = 0; i < pos; i++)
        mode /= 10;
    mode %= 10;

    int64_t* ptr = mem_get_ptr(&vm->mem, vm->ip + 1 + pos);

    switch (mode) {
        case MODE_POS:
            return mem_get_ptr(&vm->mem, *ptr);
        case MODE_IMM:
            return ptr;
        case MODE_REL:
            return mem_get_ptr(&vm->mem, vm->rb + *ptr);
        default:
            // TODO Error message
            exit(1);
    }
}

void vm_step(VM* vm) {
    int64_t inst = *mem_get_ptr(&vm->mem, vm->ip) % 100;
    int64_t* buffer;
    
    switch (inst) {
        case OP_ADD:
            *param(vm, 2) = *param(vm, 0) + *param(vm, 1);
            vm->ip += 4;
            break;
        case OP_MUL:
            *param(vm, 2) = *param(vm, 0) * *param(vm, 1);
            vm->ip += 4;
            break;
        case OP_INP:
            if (vm->input.length > 0)
                *param(vm, 0) = queue_remove_value(&vm->input, int64_t);
            else
                *param(vm, 0) = -1;
            vm->ip += 2;
            break;
        case OP_OUT:
            queue_push_value(&vm->output, *param(vm, 0));
            vm->ip += 2;
            break;
        case OP_JNZ:
            if (*param(vm, 0) != 0)
                vm->ip = *param(vm, 1);
            else
                vm->ip += 3;
            break;
        case OP_JEZ:
            if (*param(vm, 0) == 0)
                vm->ip = *param(vm, 1);
            else
                vm->ip += 3;
            break;
        case OP_TLT:
            *param(vm, 2) = *param(vm, 0) < *param(vm, 1) ? 1 : 0;
            vm->ip += 4;
            break;
        case OP_TEQ:
            *param(vm, 2) = *param(vm, 0) == *param(vm, 1) ? 1 : 0;
            vm->ip += 4;
            break;
        case OP_ARB:
            vm->rb += *param(vm, 0);
            vm->ip += 2;
            break;
        case OP_HLT:
            vm->state = VM_HALTED;
            vm->ip += 1;
            break;
        default:
            // TODO Error message
            exit(1);
    }
}

uint64_t vm_run_til_halt(VM* vm) {
    uint64_t cycles = 0;
    while (vm->state == VM_RUNNING) {
        cycles++;
        vm_step(vm);
    }
    return cycles;
}

VMState vm_run_til_event(VM* vm) {
    while (vm->state == VM_RUNNING && vm->output.length == 0)
        vm_step(vm);
    return vm->state;
}