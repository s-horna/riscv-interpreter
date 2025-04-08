# Description

This is a RISC-V VM Interpreter. It is compliant with RV32I with M extension support. It uses a linear memory model of 1MiB and 32 registers. It can run using a switch or a direct thread implementation.

# Instructions

Build the executable:
> make

I built the interpreter to either run by using a switch or a direct thread implementation (which I explained more in the speed.md document in /docs). You can initialize registers to have initial values by writing in the register number and the value at the end of the command

Running the program:
> interpreter.exe <input file> <output file (without .txt)> [-S (switch) or -G (goto) or -B (both)] <<register> <value>>

Example:
> ./interpreter.exe ../tests/primality.bin out -B 11 97 13 57 21 22

This command runs the interpreter with both the switch and goto implementations (-B flag) using the program that is in the primality.bin test and writes the output in out_switch.txt and out_goto.txt. It preloads register 11 with the value 97, register 13 with the value 57, and register 21 with the value 22.