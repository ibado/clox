#include "vm.h"
#include "chunk.h"
#include <stdio.h>

VM vm;

void vm_init() {}

void vm_free() {}

static inline u8 read_byte() { return *vm.ip++; }
static inline Value read_constant() {
  return vm.chunk->constants.values[read_byte()];
}

static VmResult run() {
  for (;;) {
    u8 instruction;
    switch (instruction = read_byte()) {
    case OP_CONSTANT: {
      Value value = read_constant();
      value_print(value);
      printf("\n");
      break;
    }
    case OP_RETURN:
      return VM_OK;
    }
  }
}

VmResult vm_exec(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}
