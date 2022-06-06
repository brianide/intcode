#pragma once

#include "assocarray.h"

#define MEMORY_CHUNK_SIZE 2048

typedef struct {
    AssocArray chunks;
} ChunkMemory;

ChunkMemory mem_create();
void mem_destroy(ChunkMemory* mem);
int64_t* mem_get_ptr(ChunkMemory* mem, size_t index);
uint64_t mem_max_index_allocated(ChunkMemory* mem);