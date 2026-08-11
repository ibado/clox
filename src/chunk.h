#ifndef clox_chunck_h
#define clox_chunck_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_RETURN,
} OpCode;

typedef struct {
  int count;
  int capacity;
  u8* code;
  int* lines;
  ValueArray constants;
} Chunk;

void chunk_init(Chunk* chunk);
void chunk_write(Chunk* chunk, u8 byte, int line);
void chunk_free(Chunk* chunk);
int chunk_add_const(Chunk* chunk, Value value);

#endif
