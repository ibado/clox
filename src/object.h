#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(vaue) (AS_OBJECT(value)->type)
#define IS_STRING(value) is_obj_type(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *)AS_OBJECT(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJECT(value))->chars)

typedef enum {
  OBJ_STRING,
} ObjectType;

struct Object {
  ObjectType type;
};

struct ObjString {
  Object object;
  int length;
  char *chars;
};

ObjString *take_string(char *chars, int len);
ObjString *copy_string(const char *chars, int length);
void object_print(Value value);

static inline bool is_obj_type(Value value, ObjectType type) {
  return IS_OBJECT(value) && AS_OBJECT(value)->type == type;
}

#endif
