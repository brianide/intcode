#include "chunkmem.h"

int64_t* ensureChunk(ChunkMemory* mem, size_t chunkIndex) {
    int64_t* chunk = assoc_get(&mem->chunks, chunkIndex);
    if (!chunk) {
        chunk = calloc(MEMORY_CHUNK_SIZE, sizeof(int64_t));
        assoc_putAt(&mem->chunks, chunkIndex, chunk);
    }
    return chunk;
}

ChunkMemory mem_create() {
    return (ChunkMemory) {
        .chunks = assoc_create()
    };
}

void mem_destroy(ChunkMemory* mem) {
    assoc_destroy(&mem->chunks, &free);
}

int64_t* mem_get_ptr(ChunkMemory* mem, size_t index) {
    int64_t* chunk = ensureChunk(mem, index / MEMORY_CHUNK_SIZE);
    return &chunk[index % MEMORY_CHUNK_SIZE];
}