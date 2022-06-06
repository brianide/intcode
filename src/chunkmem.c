#include "chunkmem.h"

int64_t* ensureChunk(ChunkMemory* mem, size_t chunkIndex) {
    int64_t* chunk = assoc_get(&mem->chunks, chunkIndex);
    if (!chunk) {
        chunk = calloc(MEMORY_CHUNK_SIZE, sizeof(int64_t));
        assoc_put(&mem->chunks, chunkIndex, chunk);
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

uint64_t mem_max_index_allocated(ChunkMemory* mem) {
    uint64_t maxkey = 0;
    for (AssocEntry* curr = assoc_begin(&mem->chunks); curr != assoc_end(&mem->chunks); curr++) {
        if (curr->key > maxkey)
            maxkey = curr->key;
    }

    return (maxkey + 1) * MEMORY_CHUNK_SIZE - 1;
}