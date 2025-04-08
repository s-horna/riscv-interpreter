#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#define MEM_SIZE (1 << 20)
#define REG_COUNT 32

#include <stdint.h>

typedef struct {
    uint32_t regs[REG_COUNT];
    uint32_t pc;
    uint8_t* memory;
    uint32_t* program;
    size_t program_size;
    int running;
    int fault;
} virtualmachine;

virtualmachine* vm_init(uint32_t* program, size_t program_size, uint32_t* initial_regs);

void vm_run(virtualmachine* vm);
void vm_run_goto(virtualmachine* vm);

void exec_R_inst(virtualmachine* vm, uint32_t instruction);
void exec_I_inst(virtualmachine* vm, uint32_t instruction);
void exec_load_inst(virtualmachine* vm, uint32_t instruction);
void exec_S_inst(virtualmachine* vm, uint32_t instruction);
void exec_B_inst(virtualmachine* vm, uint32_t instruction);
void exec_jal_inst(virtualmachine* vm, uint32_t instruction);
void exec_jalr_inst(virtualmachine* vm, uint32_t instruction);
void exec_lui_inst(virtualmachine* vm, uint32_t instruction);
void exec_auipc_inst(virtualmachine* vm, uint32_t instruction);

void write_to_reg(virtualmachine* vm, int index, uint32_t data);

void write_to_mem(virtualmachine* vm, uint32_t rel_adress, uint32_t data, int size);
uint32_t read_from_mem(virtualmachine* vm, uint32_t rel_adress, int size, int sign);

#endif