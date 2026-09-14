#include "memory.h"
#include "object.h"
#include "vm.h"

void *reallocate(void *pointer, size_t old_size, size_t new_size) {
  (void)old_size; // unused for now
  if (new_size == 0) {
    free(pointer);
    return NULL;
  }

  void *result = realloc(pointer, new_size);
  if (result == NULL) { exit(1); }
  return result;
}

static void free_object(Object *object) {
  switch (object->type) {
  case OBJ_STRING: {
    ObjString *string = (ObjString *)object;
    FREE_ARRAY(char, string->chars, string->length + 1);
    FREE(ObjString, object);
    break;
  }
  }
}

void free_objects() {
  Object *object = vm.objects;
  while (object != NULL) {
    Object *next = object->next;
    free_object(object);
    object = next;
  }
}
