#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "queue.h"

struct Node {
    int data;
    struct timeval arrival_time;
    struct Node* next;
};

struct Queue {
    int size;
    int capacity;
    struct Node* front;
    struct Node* rear;
};

Queue queueCreate(int capacity) {
    Queue q = (Queue)malloc(sizeof(struct Queue));
    if (!q) return NULL;

    q->size = 0;
    q->capacity = capacity;
    q->front = q->rear = NULL;
    return q;
}

bool isQueueEmpty(Queue q) {
    return (q->size == 0);
}

bool isQueueFull(Queue q) {
    return (q->size == q->capacity);
}

int getQueueSize(Queue q) {
    return q->size;
}

bool enqueue(Queue q, int value, struct timeval arrival) {

    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) return false;

    newNode->data = value;
    newNode->arrival_time = arrival;
    newNode->next = NULL;

    if (isQueueEmpty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    q->size++;
    return true;
}

int dequeue(Queue q) {
    if (isQueueEmpty(q)) return -1;

    struct Node* temp = q->front;
    int value = temp->data;

    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->size--;
    return value;
}

struct timeval getHeadArrivalTime(Queue q) {
    if (isQueueEmpty(q)) {
        return (struct timeval){0};
    }
    return q->front->arrival_time;
}

int find(Queue q, int value) {
    if (isQueueEmpty(q)) return -1;

    struct Node* current = q->front;
    int index = 0;

    while (current != NULL) {
        if (current->data == value) {
            return index;
        }
        current = current->next;
        index++;
    }

    return -1;
}

int dequeueAtIndex(Queue q, int index) {
    if (isQueueEmpty(q) || index < 0 || index >= q->size) {
        return -1;
    }

    if (index == 0) {
        return dequeue(q);
    }

    struct Node* prev = q->front;
    for (int i = 0; i < index - 1; i++) {
        prev = prev->next;
    }

    struct Node* toRemove = prev->next;
    int value = toRemove->data;

    prev->next = toRemove->next;
    if (toRemove == q->rear) {
        q->rear = prev;
    }

    free(toRemove);
    q->size--;
    return value;
}

void queueDestroy(Queue q) {
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }
    free(q);
}

struct timeval getArrivalTimeAtIndex(Queue q, int index) {
    if (isQueueEmpty(q) || index < 0 || index >= q->size) {
        return (struct timeval){0};
    }

    struct Node* current = q->front;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    return current->arrival_time;
}