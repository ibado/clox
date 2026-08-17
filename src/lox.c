#include "chunk.h"
#include "unity.h"
#include "vm.h"

int main() {
  vm_init();

  Chunk chunk;
  chunk_init(&chunk);

#define ADD_CONST(chunk, val, line)                                            \
  do {                                                                         \
    chunk_write((chunk), OP_CONSTANT, (line));                                 \
    int constant = chunk_add_const((chunk), (val));                            \
    chunk_write((chunk), constant, (line));                                    \
  } while (false)

  ADD_CONST(&chunk, 3, 1);
  ADD_CONST(&chunk, 2, 1);
  ADD_CONST(&chunk, 1, 1);
  chunk_write(&chunk, OP_MULTIPLY, 1);
  chunk_write(&chunk, OP_ADD, 1);
  chunk_write(&chunk, OP_RETURN, 1);

  vm_exec(&chunk);
  vm_free();
  chunk_free(&chunk);
  return 0;
}
