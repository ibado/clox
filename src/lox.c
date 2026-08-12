#include "chunk.h"
#include "debug.h"
#include "unity.h"

int main() {
  Chunk chunk;
  chunk_init(&chunk);

  chunk_write(&chunk, OP_CONSTANT, 123);
  int constant = chunk_add_const(&chunk, 1.2);
  chunk_write(&chunk, constant, 123);

  chunk_write(&chunk, OP_RETURN, 123);

  disassemble_chunk(&chunk, "test chunk");
  chunk_free(&chunk);
  return 0;
}
