#include <stdio.h>
#include <stddef.h>
#include <string.h>
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

VM* vm_create() {
    VM* vm = malloc(sizeof(VM));
    *vm = (VM) {
        .state = VM_RUNNING,
        .mem = mem_create()
    };
    return vm;
}

void vm_destroy(VM* vm) {
    if (vm->state >= VM_ERROR)
        fprintf(stderr, "##VM ERROR: %d; IP: %zu; RB: %zu ##\n", vm->state, vm->ip, vm->rb);
        
    mem_destroy(&vm->mem);
    free(vm);
}

// INPUT/OUTPUT ///////////////////////////////////////////////////////////////

static bool queue_add(IntQueue* queue, int64_t value) {
    if (queue->available == VM_QUEUE_MAX)
        return false;

    queue->buffer[queue->write] = value;
    queue->write = (queue->write + 1) % VM_QUEUE_MAX;
    queue->available++;
    return true;
}

static int64_t queue_remove(IntQueue* queue, bool* outcome) {
    if (outcome)
        *outcome = queue->available;

    if (!queue->available) {
        return -1;
    }
    
    const int64_t res = queue->buffer[queue->read];
    queue->read = (queue->read + 1) % VM_QUEUE_MAX;
    queue->available--;
    return res;
}

bool vm_push_input(VM* vm, int64_t value) {
    return queue_add(&vm->input, value);
}

bool vm_try_get_output(VM* vm, int64_t* out) {
    bool outcome;
    *out = queue_remove(&vm->output, &outcome);
    return outcome;
}

size_t vm_has_output(VM* vm) {
    return vm->output.available;
}

int64_t vm_get_output(VM* vm) {
    return queue_remove(&vm->output, NULL);
}

bool vm_output_full(VM* vm) {
    return vm->output.available == VM_QUEUE_MAX;
}

// PROGRAM LOADING ////////////////////////////////////////////////////////////

VMProgram* vm_parse_program(const char* path) {
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

    const size_t data_offset = offsetof(VMProgram, data);
    const size_t prog_size = sizeof(int64_t) * cursor;
    VMProgram* prog = malloc(data_offset + sizeof(int64_t) * cursor);
    prog->length = cursor;
    memcpy((void*) prog + data_offset, buffer, prog_size);

    free(buffer);
    return prog;
}

void vm_destroy_program(VMProgram* prog) {
    free(prog);
}

void vm_load(VM* vm, VMProgram* prog) {
    for (size_t i = 0; i < prog->length; i++) {
        *mem_get_ptr(&vm->mem, i) = prog->data[i];
    }
}

void vm_load_file(VM* vm, const char* file) {
    VMProgram* prog = vm_parse_program(file);
    vm_load(vm, prog);
    vm_destroy_program(prog);
}

// PROGRAM EXECUTION //////////////////////////////////////////////////////////

static inline int64_t next_inst(VM* vm) {
    return *mem_get_ptr(&vm->mem, vm->ip) % 100;
}

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
            vm->state = VM_INVALID_MODE;
            return mem_get_ptr(&vm->mem, 0);
    }
}

void vm_step(VM* vm) {
    int64_t inst = next_inst(vm);
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
            *param(vm, 0) = queue_remove(&vm->input, NULL);
            vm->ip += 2;
            break;
        case OP_OUT:
            if (!queue_add(&vm->output, *param(vm, 0)))
                vm->state = VM_QUEUE_OVERFLOW;
            else
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
            break;
        default:
            vm->state = VM_INVALID_INSTRUCTION;
            break;
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

VMState vm_run_til_event(VM* vm, uint8_t flags) {
    for (;;) {
        if (vm->state != VM_RUNNING)
            break;
        if (flags & VM_WAIT_BLOCK_INPUT && vm_awaiting_input(vm))
            break;
        if (flags & VM_WAIT_ANY_OUTPUT && vm_has_output(vm))
            break;
        if (flags & VM_WAIT_FULL_OUTPUT && vm_output_full(vm) && next_inst(vm) == OP_OUT)
            break;

        vm_step(vm);
    }

    return vm->state;
}

bool vm_awaiting_input(VM* vm) {
    return vm->state == VM_RUNNING && !vm->input.available && next_inst(vm) == OP_INP;
}

// Misc

void vm_dump_state(VM* vm) {
    // Print registers
    printf("IP: %zu   RB: %zu\n", vm->ip, vm->rb);

    // Print output queue
    printf("OUT: ");
    const IntQueue queue = vm->output;
    if (queue.available) {
        for (size_t i = 0; i < queue.available; i++) {
            printf("%ld", queue.buffer[(queue.read + i) % VM_QUEUE_MAX]);

            if (i + 1 == queue.available) {
                printf("\n");
                break;
            }
            else
                printf(",");
        }
    }
    else
        printf("(empty)\n");

    // Print current chunk
    int64_t* chunkptr = mem_get_ptr(&vm->mem, vm->ip / MEMORY_CHUNK_SIZE * MEMORY_CHUNK_SIZE);
    for (size_t i = 0; i < MEMORY_CHUNK_SIZE; i++) {
        if (i == vm->ip % MEMORY_CHUNK_SIZE)
            printf("[%ld]", chunkptr[i]);
        else
            printf("%ld", chunkptr[i]);

        if (i + 1 < MEMORY_CHUNK_SIZE)
            printf(" ");
        else
            printf("\n");
    }
}