#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint64_t key;
    void* data;
} AssocEntry;

typedef struct {
    size_t capacity;
    size_t size;
    AssocEntry* entries;
} AssocArray;

typedef void (*AssocDataDestructor)(void*);

AssocArray assoc_create();
void assoc_destroy(AssocArray* assoc, AssocDataDestructor destructor);
void assoc_putAt(AssocArray* assoc, uint64_t key, void* data);
void* assoc_get(AssocArray* assoc, uint64_t key);
AssocEntry* assoc_begin(AssocArray* assoc);
AssocEntry* assoc_end(AssocArray* assoc);