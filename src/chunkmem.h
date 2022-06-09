#pragma once

#include "arraylist.h"

#define MEMORY_CHUNK_SIZE 8192

typedef ArrayList ChunkMemory;

ChunkMemory* mem_create();
void mem_destroy(ChunkMemory* mem);
int64_t* mem_get_ptr(ChunkMemory** mem, size_t index);