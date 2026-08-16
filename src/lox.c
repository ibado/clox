#include "chunk.h"
#include "unity.h"
#include "vm.h"

int main() {
  vm_init();

  Chunk chunk;
  chunk_init(&chunk);

  chunk_write(&chunk, OP_CONSTANT, 123);
  int constant = chunk_add_const(&chunk, 1.2);
  chunk_write(&chunk, constant, 123);

  chunk_write(&chunk, OP_CONSTANT, 123);
  constant = chunk_add_const(&chunk, 3.4);
  chunk_write(&chunk, constant, 123);

  chunk_write(&chunk, OP_ADD, 123);

  chunk_write(&chunk, OP_CONSTANT, 123);
  constant = chunk_add_const(&chunk, 5.6);
  chunk_write(&chunk, constant, 123);

  chunk_write(&chunk, OP_DIVIDE, 123);

  chunk_write(&chunk, OP_NEGATE, 123);
  chunk_write(&chunk, OP_RETURN, 124);

  vm_exec(&chunk);
  vm_free();
  chunk_free(&chunk);
  return 0;
}
