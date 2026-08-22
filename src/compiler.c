#include "compiler.h"
#include "lexer.h"
#include <stdio.h>

void compile(const char *source) {
  lexer_init(source);
  int line = -1;
  for (;;) {
    Token token = lexer_scan();
    if (token.line != line) {
      printf("%4d ", token.line);
      line = token.line;
    } else {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.type, token.length, token.start);

    if (token.type == TOKEN_EOF)
      break;
  }
}
