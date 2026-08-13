#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "common.h"

typedef struct {
  Chunk *chunk;
  u8 *ip;
} VM;

typedef enum {
  VM_OK,
  COMPILE_ERROR,
  RUNTIME_ERROR,
} VmResult;

void vm_init();
void vm_free();
VmResult vm_exec(Chunk *chunk);

#endif
