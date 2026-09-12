#include "vm.h"
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BINARY_OP(value_type, op)                                              \
  do {                                                                         \
    if (!IS_NUMBER(stack_peek(0)) || !IS_NUMBER(stack_peek(1))) {              \
      runtime_error("Operands must be numbers");                               \
      return RUNTIME_ERROR;                                                    \
    }                                                                          \
    double b = AS_NUMBER(pop_value());                                         \
    double a = AS_NUMBER(pop_value());                                         \
    push_value(value_type(a op b));                                            \
  } while (false)

VM vm;

static void reset_stack() { vm.stack_top = vm.stack; }

static inline u8 read_byte() { return *vm.ip++; }

static inline Value read_constant() {
  return vm.chunk->constants.values[read_byte()];
}

void vm_init() { reset_stack(); }

void vm_free() {}

static void runtime_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = chunk_get_line(vm.chunk, instruction); // double check this one
  fprintf(stderr, "[line %d] in script\n", line);
  reset_stack();
}

static Value stack_peek(int distance) { return vm.stack_top[-1 - distance]; }

static bool is_falsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString *a = AS_STRING(pop_value());
  ObjString *b = AS_STRING(pop_value());

  int len = a->length + b->length;
  char *chars = ALLOCATE(char, len + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[len] = 0;

  ObjString *result = take_string(chars, len);
  push_value(OBJECT_VAL(result));
}

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
    case OP_NIL: push_value(NIL_VAL); break;
    case OP_TRUE: push_value(BOOL_VAL(true)); break;
    case OP_FALSE: push_value(BOOL_VAL(false)); break;
    case OP_EQUAL: {
      Value b = pop_value();
      Value a = pop_value();
      push_value(BOOL_VAL(value_equal(a, b)));
      break;
    }
    case OP_GREATER: BINARY_OP(BOOL_VAL, >); break;
    case OP_LESS: BINARY_OP(BOOL_VAL, <); break;
    case OP_ADD: {
      if (IS_STRING(stack_peek(0)) && IS_STRING(stack_peek(1))) {
        concatenate();
      } else if (IS_NUMBER(stack_peek(0)) && IS_NUMBER(stack_peek(1))) {
        double a = AS_NUMBER(pop_value());
        double b = AS_NUMBER(pop_value());
        push_value(NUMBER_VAL(a + b));
      } else {
        runtime_error("Operands must be two strings or two numbers");
        return RUNTIME_ERROR;
      }
      break;
    }
    case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
    case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
    case OP_DIVIDE: BINARY_OP(NUMBER_VAL, /); break;
    case OP_NOT: push_value(BOOL_VAL(is_falsey(pop_value()))); break;
    case OP_NEGATE:
      if (!IS_NUMBER(stack_peek(0))) {
        runtime_error("Operand must be a number");
        return RUNTIME_ERROR;
      }
      push_value(NUMBER_VAL(-AS_NUMBER(pop_value())));
      break;
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
