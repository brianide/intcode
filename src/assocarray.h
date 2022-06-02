#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef struct AssocEntry {
    uint64_t key;
    void* data;
} AssocEntry;

typedef void (*AssocDataDestructor)(void*);

typedef struct AssocArray {
    size_t capacity;
    size_t size;
    AssocDataDestructor destructor;
    AssocEntry* entries;
} AssocArray;

AssocArray assoc_create(AssocDataDestructor destructor);
void assoc_destroy(AssocArray* assoc);
void assoc_putAt(AssocArray* assoc, uint64_t key, void* data);
void* assoc_get(AssocArray* assoc, uint64_t key);
AssocEntry* assoc_begin(AssocArray* assoc);
AssocEntry* assoc_end(AssocArray* assoc);