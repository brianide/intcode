#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "extras.h"

typedef struct QueueNode {
    void* data;
    struct QueueNode* prev;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    size_t length;
    QueueNode* head;
    QueueNode* tail;
} Queue;

Queue queue_create();
void queue_destroy(Queue* q, DataDisposer disposer);
void queue_push(Queue* q, void* data);
void* queue_remove(Queue* q);
bool queue_try_remove(Queue* q, void** data);

#define queue_push_value(q,a) ({           \
    typeof (a) _a = a;                     \
    typeof (a)* _ptr = malloc(sizeof(_a)); \
    *_ptr = _a;                            \
    queue_push((q), _ptr);                 \
})

#define queue_remove_value(q,t) ({    \
    t* _ptr = (t*) queue_remove((q)); \
    t _val = *_ptr;                   \
    free(_ptr);                       \
    _val;                             \
})