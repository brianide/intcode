#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "extras.h"

typedef struct {
    uint64_t key;
    void* data;
} AssocEntry;

typedef struct {
    size_t capacity;
    size_t size;
    AssocEntry* entries;
} AssocArray;

AssocArray assoc_create();
void assoc_destroy(AssocArray* assoc, DataDisposer disposer);
void assoc_put(AssocArray* assoc, uint64_t key, void* data);
void* assoc_get(AssocArray* assoc, uint64_t key);
AssocEntry* assoc_begin(AssocArray* assoc);
AssocEntry* assoc_end(AssocArray* assoc);