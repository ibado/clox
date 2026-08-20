#include "lexer.h"
#include <string.h>

typedef struct {
  const char *start;
  const char *current;
  int line;
} Lexer;

Lexer lexer;

void lexer_init(const char *source) {
  lexer.start = source;
  lexer.current = source;
  lexer.line = 1;
}

static bool is_at_end() { return *lexer.current == '\0'; }

static Token make_token(TokenType type) {
  return (Token){
      .type = type,
      .start = lexer.start,
      .length = (int)(lexer.current - lexer.start),
      .line = lexer.line,
  };
}

static Token error_token(const char *msg) {
  return (Token){
      .type = TOKEN_ERROR,
      .start = msg,
      .length = (int)strlen(msg),
      .line = lexer.line,
  };
}

static char advance() {
  lexer.current++;
  return lexer.current[-1];
}

static bool match(char c) {
  if (is_at_end()) return false;
  if (*lexer.current != c) return false;
  lexer.current++;
  return true;
}

static char peek_next() {
  if (is_at_end()) return '\0';
  return lexer.current[1];
}

static void advance_until(char c) {
  while (*lexer.current != c && !is_at_end()) advance();
}

Token lexer_scan() {
  lexer.start = lexer.current;
  if (is_at_end()) return make_token(TOKEN_EOF);
  char c = advance();
  switch (c) {
  case '(': return make_token(TOKEN_RIGHT_PAREN);
  case ')': return make_token(TOKEN_LEFT_PAREN);
  case '{': return make_token(TOKEN_LEFT_BRACE);
  case '}': return make_token(TOKEN_RIGHT_BRACE);
  case ';': return make_token(TOKEN_SEMICOLON);
  case ',': return make_token(TOKEN_COMMA);
  case '.': return make_token(TOKEN_DOT);
  case '-': return make_token(TOKEN_MINUS);
  case '+': return make_token(TOKEN_PLUS);
  case '*': return make_token(TOKEN_STAR);
  case '/':
    return match('/') ? (advance_until('\n'), lexer_scan())
                      : make_token(TOKEN_SLASH);
  case '!': return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=': return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<': return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>': return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case ' ':
  case '\r':
  case '\t': return lexer_scan(); // ignore whitespace
  case '\n': {
    lexer.line++;
    return lexer_scan();
  }
  default: return error_token("Unexpected character");
  }
}
