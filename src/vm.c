#include "vm.h"
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include <stdio.h>

#define BINARY_OP(op)                                                          \
  do {                                                                         \
    Value b = pop_value();                                                     \
    Value a = pop_value();                                                     \
    push_value(a op b);                                                        \
  } while (false)

VM vm;

static void reset_stack() { vm.stack_top = vm.stack; }

static inline u8 read_byte() { return *vm.ip++; }

static inline Value read_constant() {
  return vm.chunk->constants.values[read_byte()];
}

void vm_init() { reset_stack(); }

void vm_free() {}

static VmResult run() {
  int ins_idx = 0;
  for (;;) {
#ifdef DEBUG_TRANCE_EXECUTION
    printf("          ");
    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
      printf("[");
      value_print(*slot);
      printf("]");
    }
    printf("\n");
    if (ins_idx < vm.chunk->count)
      disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    if (ins_idx >= vm.chunk->count) return VM_OK;
    OpCode instruction;
    switch (instruction = read_byte()) {
    case OP_CONSTANT: {
      Value value = read_constant();
      ins_idx++;
      push_value(value);
      break;
    }
    case OP_ADD: BINARY_OP(+); break;
    case OP_SUBTRACT: BINARY_OP(-); break;
    case OP_MULTIPLY: BINARY_OP(*); break;
    case OP_DIVIDE: BINARY_OP(/); break;
    case OP_NEGATE: push_value(-pop_value()); break;
    case OP_RETURN: {
      value_print(pop_value());
      printf("\n");
      return VM_OK;
    }
    }
    ins_idx++;
  }
}

VmResult vm_exec(const char *source) {
  Chunk chunk;
  chunk_init(&chunk);
  if (!compile(source, &chunk)) {
    chunk_free(&chunk);
    return RUNTIME_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  VmResult result = run();

  chunk_free(&chunk);

  return result;
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
