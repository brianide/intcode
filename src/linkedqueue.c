#include "linkedqueue.h"

static QueueNode* new_node(void* data, QueueNode* prev, QueueNode* next) {
    QueueNode* node = malloc(sizeof(QueueNode));
    *node = (QueueNode) { data, prev, next };
    return node;
}

Queue queue_create() {
    return (Queue) { .length = 0, .head = NULL, .tail = NULL };
}

void queue_destroy(Queue* q, DataDisposer disposer) {
    QueueNode* curr = q->head;
    while (curr) {
        if (disposer)
            disposer(curr->data);
        QueueNode* temp = curr;
        curr = curr->next;
        free(temp);
    }
}

void queue_push(Queue* q, void* data) {
    q->head = new_node(data, NULL, q->head);

    if (q->head->next)
        q->head->next->prev = q->head;

    if (!q->tail)
        q->tail = q->head;

    q->length++;
}

void* queue_remove(Queue* q) {
    void* data;

    if (queue_try_remove(q, &data))
        return data;
    return NULL;
}

bool queue_try_remove(Queue* q, void** data) {
    if (!q->tail)
        return false;

    QueueNode* removed = q->tail;
    *data = removed->data;

    q->tail = q->tail->prev;
    free(removed);

    if (!q->tail)
        q->head = NULL;

    q->length--;
    return true;
}