#include <string.h>
#include "chunkmem.h"

typedef struct {
    int64_t first_index;
    int64_t cells[MEMORY_CHUNK_SIZE];
} MemoryChunk;

ChunkMemory* mem_create() {
    return alist_create(2, sizeof(MemoryChunk));
}

void mem_destroy(ChunkMemory* mem) {
    alist_destroy(mem);
}

static MemoryChunk* ensure_chunk(ChunkMemory** mem, size_t index) {
    for (MemoryChunk* chunk = alist_begin(*mem); chunk != alist_end(*mem); chunk++) {
        if (chunk->first_index <= index && index < chunk->first_index + MEMORY_CHUNK_SIZE)
            return chunk;
    }

    MemoryChunk* chunk = alist_new_ptr(mem);
    chunk->first_index = index / MEMORY_CHUNK_SIZE * MEMORY_CHUNK_SIZE;
    memset(&chunk->cells, 0, MEMORY_CHUNK_SIZE * sizeof(int64_t));
    return chunk;
}

int64_t* mem_get_ptr(ChunkMemory** mem, size_t index) {
    MemoryChunk* chunk = ensure_chunk(mem, index);
    return &chunk->cells[index % MEMORY_CHUNK_SIZE];
}