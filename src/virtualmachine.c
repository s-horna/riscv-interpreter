#include "..\include\virtualmachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>

virtualmachine* vm_init(uint32_t* program, size_t program_size, uint32_t* initial_regs) {
    
    virtualmachine* vm = (virtualmachine*) malloc(sizeof(virtualmachine));
    if (!vm) {
        printf("Error allocating vm space");
        free(program);
        exit(1);
    }

    if (initial_regs) {
        memcpy(vm->regs, initial_regs, REG_COUNT * sizeof(initial_regs[0]));
    } else {
        memset(vm->regs, 0, REG_COUNT * sizeof(uint32_t));
    }

    vm->pc = 0;

    vm->memory = (uint8_t*) calloc(1, MEM_SIZE);
    if (vm->memory == NULL) {
        printf("Error allocating memory");
        free(vm);
        free(program);
        exit(1);
    }

    vm->program = program;
    vm->program_size = program_size;

    vm->running = 0;
    vm->fault = 0;

    return vm;
}

void vm_run(virtualmachine* vm) {
    vm->running = 1;
    while (vm->running && vm->pc < vm->program_size) {
        uint32_t instruction = vm->program[vm->pc / 4];
        switch (instruction & 0b1111111) {
            case (0b0110011):
                // clock_t start_R = clock();
                exec_R_inst(vm, instruction);
                // clock_t end_R = clock();
                // double i_millis = (((double) end_R) -  ((double) start_R)) * 1000 / CLOCKS_PER_SEC;
                // printf( "R instruction Finished in %lf ms\n", i_millis);
                break;
            case (0b0010011):
                // clock_t start_I = clock();
                exec_I_inst(vm, instruction);
                // clock_t end_I = clock();
                // double r_millis = (((double) end_I) -  ((double) start_I)) * 1000 / CLOCKS_PER_SEC;
                // printf( "I instruction Finished in %lf ms\n", r_millis);
                break;
            case (0b0000011):
                // clock_t start_load = clock();
                exec_load_inst(vm, instruction);
                // clock_t end_load = clock();
                // double load_millis = (((double) end_load) -  ((double) start_load)) * 1000 / CLOCKS_PER_SEC;
                // printf( "Load instruction Finished in %lf ms\n", load_millis);
                break;
            case (0b0100011):
                // clock_t start_S = clock();
                exec_S_inst(vm, instruction);
                // clock_t end_S = clock();
                // double s_millis = (((double) end_S) -  ((double) start_S)) * 1000 / CLOCKS_PER_SEC;
                // printf( "S instruction Finished in %lf ms\n", s_millis);
                break;
            case (0b1100011):
                // clock_t start_B = clock();
                exec_B_inst(vm, instruction);
                // clock_t end_B = clock();
                // double b_millis = (((double) end_B) -  ((double) start_B)) * 1000 / CLOCKS_PER_SEC;
                // printf( "B instruction Finished in %lf ms\n", b_millis);
                break;
            case (0b1101111):
                // clock_t start_jal = clock();
                exec_jal_inst(vm, instruction);
                // clock_t end_jal = clock();
                // double jal_millis = (((double) end_jal) -  ((double) start_jal)) * 1000 / CLOCKS_PER_SEC;
                // printf( "jal instruction Finished in %lf ms\n", jal_millis);
                break;
            case (0b1100111):
                // clock_t start_jalr = clock();
                exec_jalr_inst(vm, instruction);
                // clock_t end_jalr = clock();
                // double jalr_millis = (((double) end_jalr) -  ((double) start_jalr)) * 1000 / CLOCKS_PER_SEC;
                // printf( "jalr instruction Finished in %lf ms\n", jalr_millis);
                break;
            case (0b0110111):
                // clock_t start_lui = clock();
                exec_lui_inst(vm, instruction);
                // clock_t end_lui = clock();
                // double lui_millis = (((double) end_lui) -  ((double) start_lui)) * 1000 / CLOCKS_PER_SEC;
                // printf( "lui instruction Finished in %lf ms\n", lui_millis);
                break;
            case (0b0010111):
                // clock_t start_auipc = clock();
                exec_auipc_inst(vm, instruction);
                // clock_t end_auipc = clock();
                // double auipc_millis = (((double) end_auipc) -  ((double) start_auipc)) * 1000 / CLOCKS_PER_SEC;
                // printf( "auipc instruction Finished in %lf ms\n", auipc_millis);
                break;
            case (0b1110011):
                if (instruction & 0x100000) {
                    printf("ebreak");
                    vm->running = 0;
                }
                break;
            default:
                printf("Invalid opcode at PC=%d", vm->pc);
                vm->fault = 1;
                vm->running = 0;
                break;
        }
        vm->pc += 4;
    }
}

void exec_R_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("R instruction: %x", instruction);

    uint32_t funct7 = (instruction & 0xFE000000) >> 25;
    uint32_t rs2 = (instruction & 0x1F00000) >> 20;
    uint32_t rs1 = (instruction & 0xF8000) >> 15;
    uint32_t funct3 = (instruction & 0x7000) >> 12;
    uint32_t rd = (instruction & 0xF80) >> 7;

    switch (funct3) {
        case (0x0): {
            if (funct7 == 0x00) {
                //ADD
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                write_to_reg(vm, rd, (uint32_t) (val1 + val2));
            } else if (funct7 == 0x20) {
                //SUB
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                write_to_reg(vm, rd, (uint32_t) (val1 - val2));
            } else if (funct7 == 0x01) {
                //MUL
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                write_to_reg(vm, rd, (uint32_t) (val1 * val2));
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x1): {
            if (funct7 == 0x00) {
                //SLL
                write_to_reg(vm, rd, vm->regs[rs1] << vm->regs[rs2]);
            } else if (funct7 == 0x01) {
                //MULH
                int64_t val1 = (int64_t) ((int32_t) vm->regs[rs1]);
                int64_t val2 = (int64_t) ((int32_t) vm->regs[rs2]);
                int32_t mul = (int32_t) ((val1 * val2) >> 32);
                write_to_reg(vm, rd, (uint32_t) mul);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x2): {
            if (funct7 == 0x00) {
                //SLT
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                uint32_t val = val1 < val2 ? 1 : 0;
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x01) {
                //MULHSU
                int64_t val1 = (int64_t) ((int32_t) vm->regs[rs1]);
                int64_t val2 = (int64_t) vm->regs[rs2];
                int32_t mul = (int32_t) ((val1 * val2) >> 32);
                write_to_reg(vm, rd, (uint32_t) mul);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x3): {
            if (funct7 == 0x00) {
                //SLTU
                uint32_t val = vm->regs[rs1] < vm->regs[rs2] ? 1 : 0;
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x01) {
                //MULHU
                uint64_t val1 = (uint64_t) vm->regs[rs1];
                uint64_t val2 = (uint64_t) vm->regs[rs2];
                uint32_t mul = (uint32_t) ((val1 * val2) >> 32);
                write_to_reg(vm, rd, mul);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x4): {
            if (funct7 == 0x00) {
                //XOR
                uint32_t val = vm->regs[rs1] ^ vm->regs[rs2];
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x01) {
                //DIV
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                uint32_t div = (uint32_t) (val1 / val2);
                write_to_reg(vm, rd, div);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x5): {
            if (funct7 == 0x00) {
                //SRL
                uint32_t val = vm->regs[rs1] >> vm->regs[rs2];
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x20) {
                //SRA
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                uint32_t val = (uint32_t) (val1 >> val2);
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x01) {
                //DIVU
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                uint32_t div = (uint32_t) (val1 / val2);
                write_to_reg(vm, rd, div);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x6): {
            if (funct7 == 0x00) {
                //OR
                uint32_t val = vm->regs[rs1] | vm->regs[rs2];
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x01) {
                //REM
                int32_t val1 = (int32_t) vm->regs[rs1];
                int32_t val2 = (int32_t) vm->regs[rs2];
                uint32_t val = (uint32_t) (val1 % val2);
                write_to_reg(vm, rd, val);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case (0x7): {
            if (funct7 == 0x00) {
                //AND
                uint32_t val = vm->regs[rs1] & vm->regs[rs2];
                write_to_reg(vm, rd, val);
            } else if (funct7 == 0x01) {
                //REMU
                uint32_t val = vm->regs[rs1] % vm->regs[rs2];
                write_to_reg(vm, rd, val);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        default:
            vm->fault = 1;
            vm->running = 0;
            break;
    }
}

void exec_I_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("I instruction: %x", instruction);

    int32_t imm_offset = 0xFFF00000;
    int32_t imm = (((int32_t) instruction & imm_offset) >> 20);
    uint32_t funct7 = (instruction & 0xFE000000) >> 25;
    uint32_t rs1 = (instruction & 0xF8000) >> 15;
    uint32_t funct3 = (instruction & 0x7000) >> 12;
    uint32_t rd = (instruction & 0xF80) >> 7;

    switch (funct3) {
        case 0x0: {
            //ADDI
            int32_t val1 = (int32_t) vm->regs[rs1];
            write_to_reg(vm, rd, (uint32_t) (val1 + imm));
            break;
        }
        case 0x1: {
            if (funct7 == 0x00) {
                //SLLI
                uint32_t new_imm = imm & 0b11111;
                uint32_t val1 = vm->regs[rs1];
                write_to_reg(vm, rd, val1 << new_imm);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case 0x2: {
            //SLTI
            int32_t val1 = (int32_t) vm->regs[rs1];
            uint32_t val = val1 < imm ? 1 : 0;
            write_to_reg(vm, rd, val);
            break;
        }
        case 0x3: {
            //SLTIU
            uint32_t val1 = vm->regs[rs1];
            uint32_t val = val1 < ((uint32_t) imm) ? 1 : 0;
            write_to_reg(vm, rd, val);
            break;
        }
        case 0x4: {
            //XORI
            uint32_t val1 = vm->regs[rs1];
            write_to_reg(vm, rd, (uint32_t) (val1 ^ imm));
            break;
        }
        case 0x5: {
            if (funct7 == 0x00) {
                //SRLI
                uint32_t new_imm = imm & 0b11111;
                uint32_t val1 = vm->regs[rs1];
                write_to_reg(vm, rd, val1 >> new_imm);
            } else if (funct7 == 0x20) {
                //SRAI
                int32_t new_imm = imm & 0b11111;
                int32_t val1 = (int32_t) vm->regs[rs1];
                write_to_reg(vm, rd, val1 >> new_imm);
            } else {
                vm->fault = 1;
                vm->running = 0;
            }
            break;
        }
        case 0x6: {
            //ORI
            int32_t val1 = (int32_t) vm->regs[rs1];
            write_to_reg(vm, rd, (uint32_t) (val1 | imm));
            break;
        }
        case 0x7: {
            //ANDI
            int32_t val1 = (int32_t) vm->regs[rs1];
            write_to_reg(vm, rd, (uint32_t) (val1 & imm));
            break;
        }
        default:
            vm->fault = 1;
            vm->running = 0;
            break;
    }
}

void exec_load_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("Load instruction: %x", instruction);

    int32_t imm_offset = 0xFFF00000;
    int32_t imm = (((int32_t) instruction & imm_offset) >> 20);
    uint32_t rs1 = (instruction & 0xF8000) >> 15;
    uint32_t funct3 = (instruction & 0x7000) >> 12;
    uint32_t rd = (instruction & 0xF80) >> 7;

    switch (funct3) {
        case 0x0: {
            //LB
            uint32_t rel_ad = vm->regs[rs1] + imm;
            uint32_t val = read_from_mem(vm, rel_ad, 1, 1);
            write_to_reg(vm, rd, val);
            break;
        }
        case 0x1: {
            //LH
            uint32_t rel_ad = vm->regs[rs1] + imm;
            uint32_t val = read_from_mem(vm, rel_ad, 2, 1);
            write_to_reg(vm, rd, val);
            break;
        }
        case 0x2: {
            //LW
            uint32_t rel_ad = vm->regs[rs1] + imm;
            uint32_t val = read_from_mem(vm, rel_ad, 4, 0);
            write_to_reg(vm, rd, val);
            break;
        }
        case 0x4: {
            //LBU
            uint32_t rel_ad = vm->regs[rs1] + imm;
            uint32_t val = read_from_mem(vm, rel_ad, 1, 0);
            write_to_reg(vm, rd, val);
            break;
        }
        case 0x5: {
            //LHU
            uint32_t rel_ad = vm->regs[rs1] + imm;
            uint32_t val = read_from_mem(vm, rel_ad, 2, 0);
            write_to_reg(vm, rd, val);
            break;
        }
        default:
            vm->fault = 1;
            vm->running = 0;
            break;
    }
}

void exec_S_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("S instruction: %x", instruction);

    int32_t imm = ((int32_t) instruction & (int32_t) 0xFE000000) >> 20;
    uint32_t rs2 = (instruction & 0x1F00000) >> 20;
    uint32_t rs1 = (instruction & 0xF8000) >> 15;
    uint32_t funct3 = (instruction & 0x7000) >> 12;
    imm += ((uint32_t)(instruction & 0xF80)) >> 7;

    switch (funct3) {
        case 0x0 : {
            //SB
            uint32_t val = vm->regs[rs2];
            uint32_t rel_ad = vm->regs[rs1] + imm;
            write_to_mem(vm, rel_ad, val, 1);
            break;
        }
        case 0x1 : {
            //SH
            uint32_t val = vm->regs[rs2];
            uint32_t rel_ad = vm->regs[rs1] + imm;
            write_to_mem(vm, rel_ad, val, 2);
            break;
        }
        case 0x2 : {
            //SW
            uint32_t val = vm->regs[rs2];
            uint32_t rel_ad = vm->regs[rs1] + imm;
            write_to_mem(vm, rel_ad, val, 4);
            break;
        }
        default:
            vm->fault = 1;
            vm->running = 0;
            break;
    }
}

void exec_B_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("B instruction: %x", instruction);

    int32_t imm = ((int32_t) instruction & (int32_t) 0x80000000) >> 19;
    imm += (instruction & 0x7E000000) >> 20;
    imm += (instruction & 0xF00) >> 7;
    imm += (instruction & 0x80) << 4;
    uint32_t rs2 = (instruction & 0x1F00000) >> 20;
    uint32_t rs1 = (instruction & 0xF8000) >> 15;
    uint32_t funct3 = (instruction & 0x7000) >> 12;

    switch (funct3) {
        case 0x0: {
            //BEQ
            uint32_t val1 = vm->regs[rs1];
            uint32_t val2 = vm->regs[rs2];
            vm->pc += val1 == val2 ? (imm - 4) : 0;
            break;
        }
        case 0x1: {
            //BNE
            uint32_t val1 = vm->regs[rs1];
            uint32_t val2 = vm->regs[rs2];
            vm->pc += val1 != val2 ? (imm - 4) : 0;
            break;
        }
        case 0x4: {
            //BLT
            int32_t val1 = (int32_t) vm->regs[rs1];
            int32_t val2 = (int32_t) vm->regs[rs2];
            vm->pc += val1 < val2 ? (imm - 4) : 0;
            break;
        }
        case 0x5: {
            //BGE
            int32_t val1 = (int32_t) vm->regs[rs1];
            int32_t val2 = (int32_t) vm->regs[rs2];
            vm->pc += val1 >= val2 ? (imm - 4) : 0;
            break;
        }
        case 0x6: {
            //BLTU
            uint32_t val1 = vm->regs[rs1];
            uint32_t val2 = vm->regs[rs2];
            vm->pc += val1 < val2 ? (imm - 4) : 0;
            break;
        }
        case 0x7: {
            //BGEU
            uint32_t val1 = vm->regs[rs1];
            uint32_t val2 = vm->regs[rs2];
            vm->pc += val1 >= val2 ? (imm - 4) : 0;
            break;
        }
        default:
            vm->fault = 1;
            vm->running = 0;
            break;
    }
}

void exec_jal_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("jal instruction: %x", instruction);

    int32_t imm = ((int32_t) instruction & (int32_t) 0x80000000) >> 11;
    imm += instruction & 0xFF000;
    imm += (instruction & 0x100000) >> 9;
    imm += (instruction & 0x7FE00000) >> 20;

    uint32_t rd = (instruction & 0xF80) >> 7;

    write_to_reg(vm, rd, vm->pc + 4);
    vm->pc += imm;
}

void exec_jalr_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("jalr instruction: %x", instruction);

    int32_t imm = (((int32_t) instruction & (int32_t) 0xFFF00000) >> 20);
    // uint32_t funct7 = (instruction & 0xFE000000) >> 25;
    uint32_t rs1 = (instruction & 0xF8000) >> 15;
    // uint32_t funct3 = (instruction & 0x7000) >> 12;
    uint32_t rd = (instruction & 0xF80) >> 7;

    write_to_reg(vm, rd, vm->pc + 4);
    vm->pc = vm->regs[rs1] + imm;
}

void exec_lui_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("lui instruction: %x", instruction);

    uint32_t imm = instruction & 0xFFFFF000;
    uint32_t rd = (instruction & 0xF80) >> 7;

    write_to_reg(vm, rd, imm);
}

void exec_auipc_inst(virtualmachine* vm, uint32_t instruction) {
    // printf("auipc instruction: %x", instruction);

    int32_t imm = (int32_t) instruction & (int32_t) 0xFFFFF000;
    uint32_t rd = (instruction & 0xF80) >> 7;

    write_to_reg(vm, rd, vm->pc + imm);
}

void write_to_reg(virtualmachine* vm, int index, uint32_t data) {
    if (index == 0) {
        return;
    }
    vm->regs[index] = data;
}

void write_to_mem(virtualmachine* vm, uint32_t rel_adress, uint32_t data, int size) {
    if ((rel_adress + size) > MEM_SIZE | rel_adress < 0) {
        printf("Tried to set memory out of bounds\n");
        vm->fault = 1;
        vm->running = 0;
    }
    for (int i = 0; i < size; i++) {
        vm->memory[rel_adress + i] = data;
        data = data >> 8;
    }
}

uint32_t read_from_mem(virtualmachine* vm, uint32_t rel_adress, int size, int sign) {
    if ((rel_adress + size) > MEM_SIZE | rel_adress < 0) {
        printf("Tried to read memory out of bounds\n");
        vm->fault = 1;
        vm->running = 0;
    }
    uint32_t val = 0;
    for (int i = 0; i < size; i++) {
        val = val >> 8;
        val += vm->memory[rel_adress + i] << 24;
    }
    if (sign) {
        int32_t sval = (int32_t) val;
        sval = sval >> (8 * (4 - size));
        return (uint32_t) sval;
    } else {
        val = val >> (8 * (4 - size));
        return val;
    }
    
}