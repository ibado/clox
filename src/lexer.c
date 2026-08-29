#include "lexer.h"
#include <ctype.h>
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

static char peek() { return *lexer.current; }

static bool is_at_end() { return peek() == '\0'; }

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
  if (peek() != c) return false;
  lexer.current++;
  return true;
}

static char peek_next() {
  if (is_at_end()) return '\0';
  return lexer.current[1];
}

static void advance_until(char c) {
  while (peek() != c && !is_at_end()) {
    if (peek() == '\n') lexer.line++;
    advance();
  }
}

static Token string() {
  advance_until('"');
  if (is_at_end()) return error_token("Unterminated string");
  advance(); // consume closing quote
  return make_token(TOKEN_STRING);
}

static Token number() {
  while (isdigit(peek())) advance();
  if (peek() == '.' && isdigit(peek_next())) {
    advance(); // consume the dot
    while (isdigit(peek())) advance();
  }

  return make_token(TOKEN_NUMBER);
}

static TokenType identifier_type() {
  size_t size = lexer.current - lexer.start;
  switch (size) {
  case 2: {
    if (memcmp("or", lexer.start, size) == 0) return TOKEN_OR;
    if (memcmp("if", lexer.start, size) == 0) return TOKEN_IF;
    break;
  }
  case 3: {
    if (memcmp("and", lexer.start, size) == 0) return TOKEN_AND;
    if (memcmp("fun", lexer.start, size) == 0) return TOKEN_FUN;
    if (memcmp("for", lexer.start, size) == 0) return TOKEN_FOR;
    if (memcmp("nil", lexer.start, size) == 0) return TOKEN_NIL;
    if (memcmp("var", lexer.start, size) == 0) return TOKEN_VAR;
    break;
  }
  case 4: {
    if (memcmp("else", lexer.start, size) == 0) return TOKEN_ELSE;
    if (memcmp("this", lexer.start, size) == 0) return TOKEN_THIS;
    if (memcmp("true", lexer.start, size) == 0) return TOKEN_TRUE;
    break;
  }
  case 5: {
    if (memcmp("class", lexer.start, size) == 0) return TOKEN_CLASS;
    if (memcmp("false", lexer.start, size) == 0) return TOKEN_FALSE;
    if (memcmp("print", lexer.start, size) == 0) return TOKEN_PRINT;
    if (memcmp("super", lexer.start, size) == 0) return TOKEN_SUPER;
    if (memcmp("while", lexer.start, size) == 0) return TOKEN_WHILE;
    break;
  }
  case 6:
    if (memcmp("return", lexer.start, size) == 0) return TOKEN_RETURN;
  }

  return TOKEN_IDENTIFIER;
}

static Token identifier() {
  while (isalpha(peek()) || isdigit(peek() || peek() == '_')) advance();
  return make_token(identifier_type());
}

Token lexer_scan() {
  lexer.start = lexer.current;
  if (is_at_end()) return make_token(TOKEN_EOF);
  char c = advance();
  switch (c) {
  case '(': return make_token(TOKEN_LEFT_PAREN);
  case ')': return make_token(TOKEN_RIGHT_PAREN);
  case '{': return make_token(TOKEN_LEFT_BRACE);
  case '}': return make_token(TOKEN_RIGHT_BRACE);
  case ';': return make_token(TOKEN_SEMICOLON);
  case ',': return make_token(TOKEN_COMMA);
  case '.': return make_token(TOKEN_DOT);
  case '-': return make_token(TOKEN_MINUS);
  case '+': return make_token(TOKEN_PLUS);
  case '*': return make_token(TOKEN_STAR);
  case '!': return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=': return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<': return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>': return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case '/':
    return match('/') ? (advance_until('\n'), lexer_scan())
                      : make_token(TOKEN_SLASH);
  case '"': return string();
  case ' ':
  case '\r':
  case '\t': return lexer_scan(); // ignore whitespace
  case '\n': {
    lexer.line++;
    return lexer_scan();
  }
  default:
    return isdigit(c)   ? number()
           : isalpha(c) ? identifier()
                        : error_token("Unexpected character");
  }
}
