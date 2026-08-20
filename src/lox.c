#include "unity.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

#define ADD_CONST(chunk, val, line)                                            \
  do {                                                                         \
    chunk_write((chunk), OP_CONSTANT, (line));                                 \
    int constant = chunk_add_const((chunk), (val));                            \
    chunk_write((chunk), constant, (line));                                    \
  } while (false)

static void repl() {
  char line[1024];
  for (;;) {
    printf(">> ");
    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    vm_exec(line);
  }
}

static char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\"\n", path);
    exit(74);
  }
  fseek(file, 0L, SEEK_END);
  size_t filesize = ftell(file);
  rewind(file);

  char *buffer = malloc(filesize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\"\n", path);
    exit(74);
  }
  size_t bytes_read = fread(buffer, sizeof(char), filesize, file);
  if (bytes_read < filesize) {
    fprintf(stderr, "Could not read file \"%s\"\n", path);
    exit(74);
  }
  buffer[bytes_read] = '\0';

  fclose(file);
  return buffer;
}

static void run_file(const char *filepath) {
  char *source = read_file(filepath);
  VmResult result = vm_exec(source);
  free(source);

  if (result == COMPILE_ERROR)
    exit(65);
  if (result == RUNTIME_ERROR)
    exit(70);
}

int main(int argc, char **argv) {
  vm_init();

  switch (argc) {
  case 1:
    repl();
    break;
  case 2:
    run_file(argv[1]);
    break;
  default:
    fprintf(stderr, "Usage: clox <path>\n");
    exit(64);
  }

  vm_free();
  return 0;
}
