#include "segel.h"
#include "request.h"
#include "queue.h"


Queue waitingQueue;
Queue runningQueue;

pthread_mutex_t m_queue;
pthread_cond_t queueAvailable;
pthread_cond_t requestAvailable;


//block_flush variables


threads_stats thread_statistics;


void getargs(int *port, int* queueSize, int* threadsAmount, char* schedAlg, int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *threadsAmount = atoi(argv[2]);
    *queueSize = atoi(argv[3]);
    strcpy(schedAlg, argv[4]);
}

void* threadRoutine(void* args)
{
    int thread_id = *((int*)args);
    while(true)
    {
        pthread_mutex_lock(&m_queue);
        while(isQueueEmpty(waitingQueue))
        {
            pthread_cond_wait(&requestAvailable, &m_queue);
        }
        struct timeval arrive_time = getHeadArrivalTime(waitingQueue);
        int fd = dequeue(waitingQueue);
        struct timeval current_time;
        gettimeofday(&current_time, NULL);

        struct timeval dispatch_interval;
        timersub(&current_time, &arrive_time, &dispatch_interval);

        enqueue(runningQueue, fd, arrive_time);


        pthread_cond_signal(&requestAvailable);
        pthread_mutex_unlock(&m_queue);

        requestHandle(fd, &thread_statistics[thread_id], arrive_time, dispatch_interval);
        Close(fd);

    }
    return NULL;
}


int main(int argc, char *argv[])
{

    int listenfd, connfd, port, clientlen, threadsAmount, maxQueueSize;
    struct sockaddr_in clientaddr;
    char sched[16];
    getargs(&port, &maxQueueSize, &threadsAmount, sched, argc, argv);

    pthread_mutex_init(&m_queue, NULL);
    pthread_cond_init(&requestAvailable, NULL);
    pthread_cond_init(&queueAvailable, NULL);


    runningQueue = queueCreate(threadsAmount);
    waitingQueue = queueCreate(maxQueueSize);

    pthread_t* threads[threadsAmount];

    thread_statistics = malloc(threadsAmount * sizeof(struct Threads_stats));
    for (int i = 0; i < threadsAmount; i++) {
        thread_statistics[i].id = i;
        thread_statistics[i].stat_req = 0;
        thread_statistics[i].dynm_req = 0;
        thread_statistics[i].total_req = 0;
    }

    for (int i = 0; i < threadsAmount; ++i)
    {
        threads[i] = (pthread_t*)malloc(sizeof(pthread_t));
        int* thread_id = malloc(sizeof(int));
        *thread_id = i;
        pthread_create(threads[i], NULL, threadRoutine, (void *) thread_id);
    }

    listenfd = Open_listenfd(port);

    while (true) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        struct timeval arrived;
        gettimeofday(&arrived, NULL);

        pthread_mutex_lock(&m_queue);

        int total_queue_size = getQueueSize(waitingQueue) + getQueueSize(runningQueue);


        if (total_queue_size >= maxQueueSize) {
            if (strcmp(sched, "block") == 0) {
                while (total_queue_size >= maxQueueSize) {
                    pthread_cond_wait(&queueAvailable, &m_queue);
                    total_queue_size = getQueueSize(waitingQueue) + getQueueSize(runningQueue);
                }
            } else if (strcmp(sched, "dh") == 0) {

                if (!isQueueEmpty(waitingQueue)) {
                    int fd = dequeue(waitingQueue);
                    Close(fd);
                } else {
                    Close(connfd);
                    pthread_mutex_unlock(&m_queue);
                    continue;
                }
            } else if (strcmp(sched, "dt") == 0 || strcmp(sched, "bf") == 0) {
                if (strcmp(sched, "dt") == 0) {
                    Close(connfd);
                } else if (strcmp(sched, "bf") == 0) {
                    while (getQueueSize(runningQueue) + getQueueSize(waitingQueue) > 0) {
                        pthread_cond_wait(&queueAvailable, &m_queue);
                    }
                    Close(connfd);  // Close the connection after waiting
                }
                pthread_mutex_unlock(&m_queue);
                continue;  // Go back to accepting new connections

            } else if (strcmp(sched, "random") == 0) {
                if (isQueueEmpty(waitingQueue)) {
                    Close(connfd);
                    pthread_mutex_unlock(&m_queue);
                    continue;
                } else {
                    int to_remove = (getQueueSize(waitingQueue) + 1) / 2;
                    for (int i = 0; i < to_remove && !isQueueEmpty(waitingQueue); i++) {
                        int index = rand() % getQueueSize(waitingQueue);
                        int fd = dequeueAtIndex(waitingQueue, index);
                        Close(fd);
                    }
                }
            }else {
                Close(connfd);
                pthread_mutex_unlock(&m_queue);
                continue;
            }
        }

        enqueue(waitingQueue, connfd, arrived);
        pthread_cond_signal(&requestAvailable);
        pthread_mutex_unlock(&m_queue);

    }


}
