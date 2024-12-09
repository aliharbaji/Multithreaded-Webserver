#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <sys/time.h>

typedef struct Queue* Queue;

Queue queueCreate(int capacity);
bool isQueueEmpty(Queue q);
bool isQueueFull(Queue q);
int getQueueSize(Queue q);
bool enqueue(Queue q, int value, struct timeval arrival);
int dequeue(Queue q);
struct timeval getHeadArrivalTime(Queue q);
int find(Queue q, int value);
int dequeueAtIndex(Queue q, int index);
void queueDestroy(Queue q);
struct timeval getArrivalTimeAtIndex(Queue q, int index);

#endif // QUEUE_H
