#include <stddef.h>
#include <stdbool.h>
#include "arraylist.h"

#define GROW_FACTOR 1.5

static inline ArrayList* resize(ArrayList* list, size_t new_cap, size_t elem_size) {
    const size_t old_count = list ? list->count : 0;
    const size_t item_offset = offsetof(ArrayList, items);

    list = realloc(list, item_offset + new_cap * elem_size);
    list->elem_size = elem_size;
    list->capacity = new_cap;
    list->count = old_count;
    return list;
}

ArrayList* alist_create(size_t cap, size_t elem_size) {
    // Minimum capacity of 2 is enforced so that growth factor works properly
    return resize(NULL, cap >= 2 ? cap : 2, elem_size);
}

void alist_destroy(ArrayList* list) {
    free(list);
}

void* alist_get_ptr(ArrayList* list, size_t index) {
    return (void*) &list->items + index * list->elem_size;
}

void* alist_new_ptr(ArrayList** list) {
    if ((*list)->count == (*list)->capacity)
        *list = resize(*list, (*list)->capacity * GROW_FACTOR, (*list)->elem_size);
    
    return alist_get_ptr(*list, (*list)->count++);
}

void* alist_begin(ArrayList* list) {
    return (void*) &list->items;
}

void* alist_end(ArrayList* list) {
    return (void*) &list->items + list->count * list->elem_size;
}