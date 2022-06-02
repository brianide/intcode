#pragma once

#include "assocarray.h"

#define MEMORY_CHUNK_SIZE 2048

typedef struct {
    size_t cursor;
    AssocArray chunks;
} ChunkMemory;

ChunkMemory mem_create();
void mem_destroy(ChunkMemory* mem);
int64_t* mem_getPtr(ChunkMemory* mem, size_t index);
void mem_setCursor(ChunkMemory* mem, size_t index);
void mem_append(ChunkMemory* mem, int64_t value);
