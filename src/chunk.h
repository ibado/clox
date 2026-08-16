#ifndef clox_chunck_h
#define clox_chunck_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
} OpCode;

typedef struct {
  u8 count;
  int line;
} LineCount;

typedef struct {
  int count;
  int capacity;
  u8 *code;
  int lines_count;
  int lines_capacity;
  LineCount *lines;
  ValueArray constants;
} Chunk;

void chunk_init(Chunk *chunk);
void chunk_write(Chunk *chunk, u8 byte, int line);
void chunk_free(Chunk *chunk);
int chunk_add_const(Chunk *chunk, Value value);
int chunk_get_line(Chunk *chunk, int offset);

#endif
