#include "chunk.h"
#include "memory.h"
#include "value.h"

void chunk_init(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;

  chunk->lines_count = 0;
  chunk->lines_capacity = 0;
  chunk->lines = NULL;
  value_array_init(&chunk->constants);
}

void chunk_write(Chunk *chunk, u8 byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int old_capacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(old_capacity);
    chunk->code = GROW_ARRAY(u8, chunk->code, old_capacity, chunk->capacity);
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;

  // same for the lines
  if (chunk->lines_capacity < chunk->lines_count + 1) {
    int old_capacity = chunk->lines_capacity;
    chunk->lines_capacity = GROW_CAPACITY(old_capacity);
    chunk->lines = GROW_ARRAY(LineCount, chunk->lines, old_capacity,
                              chunk->lines_capacity);
  }
  if (chunk->lines_count > 0 &&
      chunk->lines[chunk->lines_count - 1].line == line) {
    chunk->lines[chunk->lines_count - 1].count += 1;
  } else {
    chunk->lines[chunk->lines_count].line = line;
    chunk->lines[chunk->lines_count].count = 1;
    chunk->lines_count++;
  }
}

void chunk_free(Chunk *chunk) {
  FREE_ARRAY(u8, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  value_array_free(&chunk->constants);
  chunk_init(chunk);
}

int chunk_add_const(Chunk *chunk, Value value) {
  value_array_write(&chunk->constants, value);
  return chunk->constants.count - 1;
}

int chunk_get_line(Chunk *chunk, int offset) {
  int current = 0;
  for (int i = 0; i < chunk->lines_count; i++) {
    current += chunk->lines[i].count;
    if (current > offset) {
      return chunk->lines[i].line;
    }
  }

  assert(0);
}
