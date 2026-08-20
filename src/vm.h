#ifndef clox_vm_h
#define clox_vm_h

#define STACK_MAX 256
#include "chunk.h"
#include "common.h"
#include "value.h"

typedef struct {
  Chunk *chunk;
  u8 *ip;
  Value stack[STACK_MAX];
  Value *stack_top;
} VM;

typedef enum {
  VM_OK,
  COMPILE_ERROR,
  RUNTIME_ERROR,
} VmResult;

void vm_init();
void vm_free();
VmResult vm_exec(const char *source);
void push_value(Value value);
Value pop_value();

#endif
