#ifndef __VM_H_
#define __VM_H_

#include <stddef.h>
#include <stdint.h>

typedef enum Error {
    ERR_OK = 0,
    ERR_VM_MEM,
} Error;

const char *err_as_str(Error err);

typedef uint16_t Reg;

typedef struct Registers {
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
    Reg sp;
    Reg ip;
} Registers;

typedef struct CPU {
    Registers regs;
} CPU;

typedef uint8_t Stack[1024];  // 1KB

// typedef enum Opcode {
//     NOP = 0x00,
//     MOV,
// } Opcode;

typedef uint16_t Opcode;

#define NOP 0x00
#define MOV 0x01

size_t inst_map(Opcode op);

// typedef struct InstructionMap {
//     Opcode op;
//     size_t size;
// } InstructionMap;

typedef uint16_t Arg;

typedef struct Instruction {
    Opcode op;
    Arg args[];  // 0-2 arguments (like x86 ISA)
} Instruction;

typedef Instruction *Program;

/*
    - 16 bit
    - AX
    - BX
    - CX
    - DX
    - SP
    - IP
    - 64KB memory
    - Serial COM port
    - (Floppy drive)
*/
typedef struct VM {
    CPU cpu;
    Stack stack;
    Program program;
} VM;

VM *vm_init(Program program, size_t size);
void vm_drop(VM *vm);

int main(int argc, char **argv);

#endif
