#include <stdio.h>
#include "modes.h"

typedef struct {
    uint32_t id;
    size_t num_operands;
    const char name[3];
} Opcode;

typedef struct {
    size_t position;
    size_t target_label;
    const Opcode* opcode;
    int64_t operands[3];
    int32_t operand_modes[3];
} Instruction;

#define MODE_POS 0
#define MODE_IMM 1
#define MODE_REL 2

static const Opcode OPCODES[] = {
    {  0, 1, "DB " },
    {  1, 3, "ADD" },
    {  2, 3, "MUL" },
    {  3, 1, "INP" },
    {  4, 1, "OUT" },
    {  5, 2, "JNZ" },
    {  6, 2, "JEZ" },
    {  7, 3, "TLT" },
    {  8, 3, "TEQ" },
    {  9, 1, "ARB" },
    { 99, 0, "HLT" },
};

const Opcode* find_opcode(int64_t inst) {
    inst %= 100;
    for (size_t i = 1; i < sizeof(OPCODES) / sizeof(Opcode); i++) {
        if (OPCODES[i].id == inst)
            return &OPCODES[i];
    }
    return &OPCODES[0];
}

void disas(VMProgram* prog) {
    Instruction* insts = calloc(prog->length, sizeof(Instruction));
    size_t inst_index = 0;

    size_t* labels = calloc(prog->length, sizeof(size_t));
    size_t label_index = 0;

    size_t cursor = 0;
    while (cursor < prog->length) {
        int64_t val = prog->data[cursor];
        int64_t modes = val / 100;

        Instruction inst;
        inst.position = cursor;
        inst.opcode = find_opcode(val);

        if (inst.opcode->id == 0) {
            inst.operands[0] = val;
            inst.operand_modes[0] = MODE_IMM;
        }
        else {
            for (size_t i = 0; i < inst.opcode->num_operands; i++) {
                inst.operands[i] = prog->data[cursor + 1 + i];
                inst.operand_modes[i] = modes % 10;
                modes /= 10;
            }
        }

        if ((inst.opcode->id == 5 || inst.opcode->id == 6) && inst.operand_modes[1] == MODE_IMM) {
            inst.target_label = SIZE_MAX;

            for (size_t m = 0; m < label_index; m++) {
                if (labels[m] == inst.operands[1])
                    inst.target_label = m;
            }

            if (inst.target_label == SIZE_MAX) {
                inst.target_label = label_index;
                labels[label_index] = inst.operands[1];
                label_index++;
            }
        }

        insts[inst_index++] = inst;
        cursor += 1 + inst.opcode->num_operands;
    }

    for (size_t i = 0; i < inst_index; i++) {
        Instruction inst = insts[i];

        // Print label, if necessary
        for (size_t l = 0; l < label_index; l++) {
            if (labels[l] == inst.position) {
                printf("label%zu:\n", l);
            }
        }

        printf("%8zd  %s  ", inst.position, inst.opcode->name);

        for (size_t a = 0; a < inst.opcode->num_operands; a++) {
            if (a == 1 && (inst.opcode->id == 5 || inst.opcode->id == 6) && inst.operand_modes[a] == MODE_IMM) {
                printf("label%zu", inst.target_label);
            }
            else {
                switch (inst.operand_modes[a]) {
                    case MODE_POS:
                        printf("[%ld]", inst.operands[a]);
                        break;
                    case MODE_IMM:
                        printf("%ld", inst.operands[a]);
                        break;
                    case MODE_REL:
                        printf("rb%+ld", inst.operands[a]);
                        break;
                }
            }

            if (a + 1 < inst.opcode->num_operands)
                printf(", ");
        }

        printf("\n");
    }

    free(labels);
    free(insts);
}