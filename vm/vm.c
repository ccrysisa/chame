#include "vm.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static InstructionMap inst_map[] = {
//     {.op = NOP, .size = 1},
//     {.op = MOV, .size = 3},
// };

// Record the number of uint16 of each instructions.
static const size_t INST_MAP[] = {
    [NOP] = 1,
    [MOV] = 3,
};

// Record the number of bytes of each instructions.
size_t inst_map(Opcode op)
{
    return INST_MAP[op] * sizeof(uint16_t);
}

const char *err_as_str(Error err)
{
    switch (err) {
    case ERR_OK:
        return "ok\n";
    case ERR_VM_MEM:
        return "ERR_VM_MEM\n";
    default:
        assert(false && "Unreachable");
        break;
    }
}

VM *vm_init(Program program, size_t size)
{
    assert(program && size);

    VM *vm = malloc(sizeof(VM));
    if (vm == NULL) {
        fprintf(stderr, "Error: %s", err_as_str(ERR_VM_MEM));
        exit(ERR_VM_MEM);
    }

    vm->program = malloc(size);
    if (vm->program == NULL) {
        free(vm);
        fprintf(stderr, "Error: %s", err_as_str(ERR_VM_MEM));
        exit(ERR_VM_MEM);
    }
    memcpy(vm->program, program, size);

    return vm;
}

void vm_drop(VM *vm)
{
    free(vm->program);
    vm->program = NULL;
    free(vm);
    vm = NULL;
}

// static Instruction example[] = {
//     {.op = mov, .args = {0x00, 0x05}},
//     {.op = nop, .args = NOARGS},
// };

static Program generate_example(void)
{
    size_t size = inst_map(MOV) + inst_map(NOP);
    Instruction *prog = malloc(size);
    if (prog == NULL) {
        fprintf(stderr, "Error: %s", err_as_str(ERR_VM_MEM));
        exit(ERR_VM_MEM);
    }

    Instruction *inst1 = prog;
    inst1->op = MOV;
    inst1->args[0] = 0x00;
    inst1->args[1] = 0x05;

    Instruction *inst2 = (Instruction *) ((char *) prog + inst_map(MOV));
    inst2->op = NOP;

    return prog;
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("Hello, world\n");

    Program prog = generate_example();
    printf("program: %p\n", (void *) prog);
    VM *vm = vm_init(prog, 2);
    printf("vm:      %p\n", (void *) vm);
    vm_drop(vm);

    return 0;
}
