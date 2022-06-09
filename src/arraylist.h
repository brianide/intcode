#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    size_t elem_size;
    size_t capacity;
    size_t count;
    void* items;
} ArrayList;

ArrayList* alist_create(size_t cap, size_t elem_size);
void alist_destroy(ArrayList* list);
void* alist_get_ptr(ArrayList* list, size_t index);
void* alist_new_ptr(ArrayList** list);
void* alist_begin(ArrayList* list);
void* alist_end(ArrayList* list);