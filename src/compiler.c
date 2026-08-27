#include "compiler.h"
#include "chunk.h"
#include "lexer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  Token previous;
  Token current;
  bool had_error;
  bool panic_mode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT, // =
  PREC_OR,         // or
  PREC_AND,        // and
  PREC_EQUIALITY,  // == !=
  PREC_COMPARISON, // < > <= =>
  PREC_TERM,       // + -
  PREC_FACTOR,     // * /
  PREC_UNARY,      // ! -
  PREC_CALL,       // . ()
  PREC_PRIMARY,
} Precedence;

typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

Parser parser;
Chunk *compiling_chunk;

static Chunk *current_chunk() { return compiling_chunk; }

static void error_at(Token *token, const char *msg) {
  if (parser.panic_mode) return;
  parser.panic_mode = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type != TOKEN_ERROR) {
    fprintf(stderr, "at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", msg);
  parser.had_error = true;
}

static void error(const char *msg) { error_at(&parser.previous, msg); }

static void error_at_current(const char *msg) {
  error_at(&parser.current, msg);
}

static void parser_advance() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = lexer_scan();
    if (parser.current.type != TOKEN_ERROR) break;

    error_at_current(parser.current.start);
  }
}

static void consume(TokenType tt, const char *msg) {
  if (parser.current.type == tt) {
    parser_advance();
    return;
  }

  error_at_current(msg);
}

static void emit_byte(u8 byte) {
  chunk_write(current_chunk(), byte, parser.previous.line);
}

static void emit_bytes(u8 b1, u8 b2) {
  emit_byte(b1);
  emit_byte(b2);
}

static void emit_return() { emit_byte(OP_RETURN); }

static void end_compiler() { emit_return(); }

static void expression();
static ParseRule *get_rule(TokenType type);
static void parse_precedence(Precedence precedence);

static u8 make_constant(double value) {
  int constidx = chunk_add_const(compiling_chunk, value);
  if (constidx >= 255) {
    error("");
    return 0;
  }

  return (u8)constidx;
}

static void emit_constant(double value) {
  emit_bytes(OP_CONSTANT, make_constant(value));
}

static void parse_precedence(Precedence precedence) {}

static void expression() { parse_precedence(PREC_ASSIGNMENT); }

static void parse_number() {
  double value = strtod(parser.previous.start, NULL);
  emit_constant(value);
}

static void parse_unary() {
  TokenType operator_type = parser.previous.type;

  parse_precedence(PREC_UNARY); // compile operand
  // emit the operator instruction
  if (operator_type == TOKEN_MINUS) emit_byte(OP_NEGATE);
}

static void parse_binary() {
  TokenType operator_type = parser.previous.type;
  ParseRule *rule = get_rule(operator_type);
  parse_precedence((Precedence)(rule->precedence + 1));

  switch (operator_type) {
  case TOKEN_PLUS: emit_byte(OP_ADD); break;
  case TOKEN_MINUS: emit_byte(OP_SUBTRACT); break;
  case TOKEN_STAR: emit_byte(OP_MULTIPLY); break;
  case TOKEN_SLASH: emit_byte(OP_DIVIDE); break;
  default: return; // unreachable
  }
}

static void parse_grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {parse_grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {parse_unary, parse_binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, parse_binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, parse_binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, parse_binary, PREC_FACTOR},
    [TOKEN_BANG] = {NULL, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {parse_number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {NULL, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static ParseRule *get_rule(TokenType type) { return &rules[type]; }

bool compile(const char *source, Chunk *chunk) {
  lexer_init(source);
  compiling_chunk = chunk;
  parser.had_error = false;
  parser.panic_mode = false;

  parser_advance();
  expression();
  consume(TOKEN_EOF, "Expected end of expression");
  end_compiler();
  return !parser.had_error;
}
