#ifndef clox_hashtable_h
#define clox_hashtable_h

#include "common.h"
#include "value.h"

typedef struct {
  ObjString *key;
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry *entries;
} Hashtable;

void hashtable_init(Hashtable *ht);
void hashtable_free(Hashtable *ht);
bool hashtable_get(Hashtable *ht, ObjString *key, Value *out);
bool hashtable_set(Hashtable *ht, ObjString *key, Value value);
bool hashtable_del(Hashtable *ht, ObjString *key);
void hashtable_set_all(Hashtable *src, Hashtable *dst);
ObjString *hashtable_find_key(Hashtable *ht, const char *key, int len, u32 hash);

#endif
