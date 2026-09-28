#include "hashtable.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include <string.h>

#define HT_LOAD_FACTOR 0.75

static Entry *find_entry(Entry *entries, int capacity, struct ObjString *key) {
  u32 index = key->hash % capacity;
  Entry *tombstone = NULL;
  for (;;) {
    Entry *entry = &entries[index];
    if (entry->key == key) return entry;
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) return tombstone == NULL ? entry : tombstone;
      else if (tombstone == NULL) tombstone = entry;
    }
    index = (index + 1) % capacity;
  }
}

static void adjust_capacity(Hashtable *ht, int capacity) {
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  ht->count = 0;
  for (int i = 0; i < ht->capacity; i++) {
    Entry *old_entry = &ht->entries[i];
    if (old_entry->key == NULL) continue;
    Entry *new_entry = find_entry(entries, capacity, old_entry->key);
    new_entry->key = old_entry->key;
    new_entry->value = old_entry->value;
    ht->count++;
  }

  FREE_ARRAY(Entry, ht->entries, ht->capacity);
  ht->entries = entries;
  ht->capacity = capacity;
}

void hashtable_init(Hashtable *ht) {
  ht->capacity = 0;
  ht->count = 0;
  ht->entries = NULL;
}

void hashtable_free(Hashtable *ht) {
  FREE_ARRAY(Entry, ht->entries, ht->capacity);
  hashtable_init(ht);
}

bool hashtable_get(Hashtable *ht, ObjString *key, Value *out) {
  if (ht->count == 0) return false;
  Entry *entry = find_entry(ht->entries, ht->capacity, key);
  if (entry->key == NULL) return false;
  *out = entry->value;
  return true;
}

bool hashtable_set(Hashtable *ht, ObjString *key, Value value) {
  if (ht->count + 1 > ht->capacity * HT_LOAD_FACTOR) {
    int new_capacity = GROW_CAPACITY(ht->capacity);
    adjust_capacity(ht, new_capacity);
  }
  Entry *entry = find_entry(ht->entries, ht->capacity, key);
  bool is_new = entry->key == NULL;
  if (is_new && IS_NIL(entry->value)) ht->count++;
  entry->key = key;
  entry->value = value;
  return is_new;
}

bool hashtable_del(Hashtable *ht, ObjString *key) {
  if (ht->count == 0) return false;
  Entry *entry = find_entry(ht->entries, ht->capacity, key);
  if (entry->key == NULL) return false;
  entry->key = NULL;
  entry->value = BOOL_VAL(true); // tombstone
  return true;
}

void hashtable_set_all(Hashtable *src, Hashtable *dst) {
  for (int i = 0; i < src->capacity; i++) {
    Entry *entry = &src->entries[i];
    if (entry->key != NULL) { hashtable_set(dst, entry->key, entry->value); }
  }
}

ObjString *hashtable_find_key(Hashtable *ht, const char *key, int len,
                              u32 hash) {
  if (ht->count == 0) return NULL;
  u32 index = hash % ht->capacity;
  for (;;) {
    Entry *entry = &ht->entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) return NULL;
    } else if (entry->key->length == len && entry->key->hash == hash &&
               memcmp(entry->key->chars, key, len) == 0) {
      return entry->key;
    }
    index = (index + 1) % ht->capacity;
  }
}
