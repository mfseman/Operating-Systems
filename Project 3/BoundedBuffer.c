// Marco Seman and Jason Lu
// Project 3 - BoundedBuffer

/*
 * boundedBuffer.c
 *
 * A complete example of simple producer/consumer program. The Producer
 * and Consumer functions are executed as independent threads.  They
 * share access to a single buffer, data.  The producer deposits a sequence
 * of integers from 1 to numIters into the buffer.  The Consumer fetches
 * these values and adds them.  Two semaphores,empty and full are used to
 * ensure that the producer and consumer alternate access to the buffer.
 *
 * SOURCE: adapted from example code in "Multithreaded, Parallel, and
 *         Distributed Programming" by Gregory R. Andrews.
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define SHARED 1

void *Producer (void *); // the two threads
void *Consumer (void *);

sem_t empty, full, mutex;       //global semaphores
int *data;                	// shared buffer, size = 1	
int numIters, bufferSize;	//Initalize the size of the buffer
int front = 0, rear = 0;	

// main() -- read command line and create threads
int main(int argc, char *argv[]) {
    pthread_t pid[3];		//pid array for producer
    pthread_t cid[3];		//cid array for customer
    
    if (argc < 3) {
        printf("Usage: boundedBuffer <Number of Iterations>\n");
        exit(0);
    }
    numIters = atoi(argv[1]);
    bufferSize = atoi(argv[2]);
//Data pointer pts to the dynamic buffer array
    data = (int*)malloc(bufferSize * sizeof(int));
    
    sem_init(&empty, SHARED, bufferSize);    // sem empty = 1
    sem_init(&full, SHARED, 0); //sem full = 0
    sem_init(&mutex, SHARED, 1); //initialize mutex to allow 1 thread access
    
    pthread_create(&(pid[1]), NULL, Producer, NULL);
    pthread_create(&(pid[2]), NULL, Producer, NULL);
    pthread_create(&(pid[3]), NULL, Producer, NULL);
    
    pthread_create(&(cid[1]), NULL, Consumer, NULL);
    pthread_create(&(cid[2]), NULL, Consumer, NULL);
    pthread_create(&(cid[3]), NULL, Consumer, NULL);
    
    pthread_join(pid[1], NULL);
    pthread_join(cid[1], NULL);
    pthread_join(pid[2], NULL);
    pthread_join(cid[2], NULL);
    pthread_join(pid[3], NULL);
    pthread_join(cid[3], NULL);
//Free the memory that was allocated by the malloc buffer
	free(data);
    pthread_exit(0);
}

// deposit 1, ..., numIters into the data buffer
void *Producer(void *arg) {
    int produced;
    
    for (produced = 0; produced < numIters; produced++) {
//Waiting for the buffer to be empty
        sem_wait(&empty);
//Only allow 1 producer to full in the buffer
        sem_wait(&mutex);
//Updater the producer index(buffer)
	front = (front+1)%bufferSize;
	data[front] = produced;
//Producer out, allows another producer to get in
        sem_post(&mutex);
//signal cusomter the buffer is full
        sem_post(&full);
    }
}

//fetch numIters items from the buffer and sum them
void *Consumer(void *arg) {
    int total = 0;
    int consumed;
    for (consumed = 0; consumed < numIters; consumed++) {
//Customer wait for the buffer to get full
        sem_wait(&full);
//Only allows one customer to get in
        sem_wait(&mutex);
//update the customer index(buffer)
 	rear = (rear+1) % bufferSize;
	total = total + data[rear];
//Customer get out, another customer will get in
        sem_post(&mutex);
//Signal the producer the buffer is empty
        sem_post(&empty);
    }
    printf("the total is %d\n", total);
}

