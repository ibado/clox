#include <stdio.h>
#include <string.h>

#include "hashtable.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type)                                        \
  (type *)allocate_object(sizeof(type), object_type)

static Object *allocate_object(size_t size, ObjectType type) {
  Object *object = (Object *)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString *allocate_string(char *chars, int length, u32 hash) {
  ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  string->hash = hash;
  hashtable_set(&vm.strings, string, NIL_VAL);
  return string;
}

// FNV-1a - same that I used in my-kv-store :D
static u32 hash_string(const char *key, int len) {
  u32 hash = 2166136261u;
  for (int i = 0; i < len; i++) {
    hash ^= (u32)key[i];
    hash *= 16777619;
  }

  return hash;
}

ObjString *take_string(char *chars, int len) {
  u32 hash = hash_string(chars, len);
  ObjString *interned = hashtable_find_key(&vm.strings, chars, len, hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, len + 1);
    return interned;
  }
  return allocate_string(chars, len, hash);
}

ObjString *copy_string(const char *chars, int len) {
  u32 hash = hash_string(chars, len);
  ObjString *interned = hashtable_find_key(&vm.strings, chars, len, hash);
  if (interned != NULL) return interned;
  char *heap_chars = ALLOCATE(char, len + 1);
  memcpy(heap_chars, chars, len);
  heap_chars[len] = '\0';
  return allocate_string(heap_chars, len, hash);
}

void object_print(Value value) {
  switch (OBJ_TYPE(value)) {
  case OBJ_STRING: printf("%s", AS_CSTRING(value)); break;
  }
}
