#include "assocarray.h"

#define ASSOC_DEFAULT_CAPACITY 8
#define ASSOC_GROW_FACTOR 1.5

AssocArray assoc_create() {
    return (AssocArray) {
        .capacity = ASSOC_DEFAULT_CAPACITY,
        .size = 0,
        .entries = calloc(ASSOC_DEFAULT_CAPACITY, sizeof(AssocEntry))
    };
}

void assoc_destroy(AssocArray* assoc, AssocDataDestructor destructor) {
    if (destructor)
        for (size_t i = 0; i < assoc->size; i++)
            destructor(assoc->entries[i].data);
    free(assoc->entries);
}

void assoc_putAt(AssocArray* assoc, uint64_t key, void* data) {
    if (assoc->size == assoc->capacity) {
        assoc->size *= ASSOC_GROW_FACTOR;
        assoc->entries = realloc(assoc->entries, assoc->capacity * sizeof(AssocEntry));
    }

    assoc->entries[assoc->size++] = (AssocEntry) { .key = key, .data = data };
}

void* assoc_get(AssocArray* assoc, uint64_t key) {
    for (size_t i = 0; i < assoc->size; i++)
        if (assoc->entries[i].key == key)
            return assoc->entries[i].data;
    return NULL;
}

AssocEntry* assoc_begin(AssocArray* assoc) {
    return assoc->entries;
}

AssocEntry* assoc_end(AssocArray* assoc) {
    return &assoc->entries[assoc->size];
}
