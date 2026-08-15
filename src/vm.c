#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include <stdio.h>

VM vm;

static void reset_stack() { vm.stack_top = vm.stack; }

void vm_init() { reset_stack(); }

void vm_free() {}

static inline u8 read_byte() { return *vm.ip++; }
static inline Value read_constant() {
  return vm.chunk->constants.values[read_byte()];
}

static VmResult run() {
  for (;;) {
#ifdef DEBUG_TRANCE_EXECUTION
    printf("          ");
    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
      printf("[");
      value_print(*slot);
      printf("]");
    }
    printf("\n");
    disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    u8 instruction;
    switch (instruction = read_byte()) {
    case OP_CONSTANT: {
      Value value = read_constant();
      push_value(value);
      break;
    }
    case OP_RETURN: {
      value_print(pop_value());
      printf("\n");
      return VM_OK;
    }
    }
  }
}

VmResult vm_exec(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

void push_value(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop_value() {
  // if (vm.stack_top == vm.stack) => empty stack
  vm.stack_top--;
  return *vm.stack_top;
}
