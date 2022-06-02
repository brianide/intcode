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

VM vm_create() {
    return (VM) {
        .ip = 0,
        .rb = 0,
        .halted = false,
        .mem = mem_create()
    };
}

void vm_destroy(VM* vm) {
    mem_destroy(&vm->mem);
}

void vm_load(VM* vm, const char* filename) {
    FILE* f = fopen(filename, "r");
    int64_t value;

    vm->mem.cursor = 0;
    while (!feof(f) && fscanf(f, "%ld,", &value) == 1) {
        mem_append(&vm->mem, value);
    }
    fclose(f);
}

int64_t* param(VM* vm, size_t pos) {
    int64_t mode = *mem_getPtr(&vm->mem, vm->ip) / 100;
    for (size_t i = 0; i < pos; i++)
        mode /= 10;
    mode %= 10;

    int64_t* ptr = mem_getPtr(&vm->mem, vm->ip + 1 + pos);

    switch (mode) {
        case MODE_POS:
            return mem_getPtr(&vm->mem, *ptr);
        case MODE_IMM:
            return ptr;
        case MODE_REL:
            return mem_getPtr(&vm->mem, vm->rb + *ptr);
        default:
            // TODO Error message
            exit(1);
    }
}

void vm_step(VM* vm) {
    int64_t inst = *mem_getPtr(&vm->mem, vm->ip) % 100;
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
            *param(vm, 0) = 0;
            vm->ip += 2;
            break;
        case OP_OUT:
            vm->ip += 2;
            break;
        case OP_JNZ:
            if (*param(vm, 0) != 0) vm->ip = *param(vm, 1);
            vm->ip += 3;
            break;
        case OP_JEZ:
            if (*param(vm, 0) == 0) vm->ip = *param(vm, 1);
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
            vm->halted = true;
            vm->ip += 1;
            break;
        default:
            // TODO Error message
            exit(1);
    }
}

uint64_t vm_runUntilHalt(VM* vm) {
    uint64_t cycles = 0;
    while (!vm->halted) {
        cycles++;
        vm_step(vm);
    }
    return cycles;
}