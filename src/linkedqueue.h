#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

typedef void (*QueueDataDestructor)(void*);

Queue queue_create();
void queue_destroy(Queue* q, QueueDataDestructor destructor);
void queue_push(Queue* q, void* data);
void* queue_remove(Queue* q);
bool queue_try_remove(Queue* q, void** data);