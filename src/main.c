#include "..\include\virtualmachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Usage: %s <input file> <output file> [-S (switch) or -G (goto) or -B (both)] <<register> <value>> \n Example: %s input.bin output.txt -B 5 2 6 10", argv[0], argv[0]); return 1;
    }

    uint32_t initial_regs[32] = {0};
    int mode = 0;

    if (argc > 3) {
        int offset = 0;
        if (strcmp(argv[3], "-S") == 0) {
            offset = 1;
            mode = 0;
        }
        if (strcmp(argv[3], "-G") == 0) {
            offset = 1;
            mode = 1;
        }
        if (strcmp(argv[3], "-B") == 0) {
            offset = 1;
            mode = 2;
        }
        for (int i = 0; i < argc - 3 - offset; i += 2) {
            if (i + 1 < argc - 3 - offset) {
                initial_regs[atoi(argv[i + 3 + offset])] = atoi(argv[i + 4 + offset]);
            }
        }
    }

    FILE* input = fopen(argv[1], "rb");
    if (!input) {
        printf("Error opening input file\n");
        exit(1);
    }

    fseek(input, 0L, SEEK_END);
    size_t program_sz = (size_t) ftell(input);
    fseek(input, 0L, SEEK_SET);

    uint32_t* program_bin = (uint32_t*) malloc(program_sz);
    if (!program_bin) {
        printf("Failed to load program\n");
        exit(1);
    }
    fread(program_bin, program_sz, 1, input);

    if (mode == 0) {
        virtualmachine* vm = vm_init(program_bin, program_sz, initial_regs);
        clock_t begin_switch = clock();
        vm_run(vm);
        clock_t end_switch = clock();
        printf("Using switch: %lf seconds\n", (((double) end_switch) - ((double) begin_switch)) / CLOCKS_PER_SEC);

        char output_file_switch[strlen(argv[2]) + 11];
        strcpy(output_file_switch, argv[2]);
        strcat(output_file_switch, "_switch.txt");
        FILE* output_switch = fopen(output_file_switch, "w");
        if (!output_switch) {
            printf("Error opening output file (switch)\n");
            exit(1);
        }

        fprintf(output_switch, "PC and register values from switch interpreter (in hex): \n \n");
        fprintf(output_switch, "PC = %x \n", vm->pc);
        int *ptr = vm->regs;
        for (int i = 0; i < REG_COUNT; i++) {
            fprintf(output_switch, "regs[%d] = %x \n", i, *ptr++);
        }
        fprintf(output_switch, "Ended with fault: %d", vm->fault);

        free(vm->memory);
        free(vm);
    } else if (mode == 1) {
        virtualmachine* vm_goto = vm_init(program_bin, program_sz, initial_regs);
        clock_t begin_goto = clock();
        vm_run_goto(vm_goto);
        clock_t end_goto = clock();
        printf("Using goto: %lf seconds\n", (((double) end_goto) - ((double) begin_goto)) / CLOCKS_PER_SEC);

        char output_file_goto[strlen(argv[2]) + 9];
        strcpy(output_file_goto, argv[2]);
        strcat(output_file_goto, "_goto.txt");

        FILE* output_goto = fopen(output_file_goto, "w");
        if (!output_goto) {
            printf("Error opening output file (goto)\n");
            exit(1);
        }

        fprintf(output_goto, "PC and register values from goto interpreter (in hex): \n \n");
        fprintf(output_goto, "PC = %x \n", vm_goto->pc);
        int *ptr = vm_goto->regs;
        for (int i = 0; i < REG_COUNT; i++) {
            fprintf(output_goto, "regs[%d] = %x \n", i, *ptr++);
        }
        fprintf(output_goto, "Ended with fault: %d", vm_goto->fault);

        free(vm_goto->memory);
        free(vm_goto);
    } else {
        virtualmachine* vm = vm_init(program_bin, program_sz, initial_regs);
        clock_t begin_switch = clock();
        vm_run(vm);
        clock_t end_switch = clock();
        printf("Using switch: %lf seconds\n", (((double) end_switch) - ((double) begin_switch)) / CLOCKS_PER_SEC);

        virtualmachine* vm_goto = vm_init(program_bin, program_sz, initial_regs);
        clock_t begin_goto = clock();
        vm_run_goto(vm_goto);
        clock_t end_goto = clock();
        printf("Using goto: %lf seconds\n", (((double) end_goto) - ((double) begin_goto)) / CLOCKS_PER_SEC);

        char output_file_switch[strlen(argv[2]) + 11];
        char output_file_goto[strlen(argv[2]) + 9];
        strcpy(output_file_switch, argv[2]);
        strcpy(output_file_goto, argv[2]);
        strcat(output_file_switch, "_switch.txt");
        strcat(output_file_goto, "_goto.txt");

        FILE* output_switch = fopen(output_file_switch, "w");
        FILE* output_goto = fopen(output_file_goto, "w");
        if (!output_switch) {
            printf("Error opening output file (switch)\n");
            exit(1);
        }
        if (!output_goto) {
            printf("Error opening output file (goto)\n");
            exit(1);
        }

        fprintf(output_switch, "PC and register values from switch interpreter (in hex): \n \n");
        fprintf(output_switch, "PC = %x \n", vm->pc);
        int* ptr = vm->regs;
        for (int i = 0; i < REG_COUNT; i++) {
            fprintf(output_switch, "regs[%d] = %x \n", i, *ptr++);
        }
        fprintf(output_switch, "Ended with fault: %d", vm->fault);

        fprintf(output_goto, "PC and register values from goto interpreter (in hex): \n \n");
        fprintf(output_goto, "PC = %x \n", vm_goto->pc);
        int* ptr_goto = vm_goto->regs;
        for (int i = 0; i < REG_COUNT; i++) {
            fprintf(output_goto, "regs[%d] = %x \n", i, *ptr_goto++);
        }
        fprintf(output_goto, "Ended with fault: %d", vm_goto->fault);

        free(vm->memory);
        free(vm);
        free(vm_goto->memory);
        free(vm_goto);
    }

    free(program_bin);

    return 0;
}