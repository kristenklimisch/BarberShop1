#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CUSTOMERS 25

void *customer (void *num);
void *barber(void *);

sem_t numChairs;


int main (int argc, char *argv[]) {
    // ID for barber thread.
    pthread_t btid;

    // IDs for customer threads.
    pthread_t tid[NUM_CUSTOMERS];

    int n, check;

    // Accept user input for the number of chairs in the waiting room.
    printf("Enter the number of chairs in barbershop waiting room: ");
    scanf("%d", &n);

    // Perform error checking for user input. 
    if ((check != 1) || (n < 0)){
        printf("Invalid entry - number of chairs must be a positive number."\n
        "Program exiting."\n);
        exit(-1);
    }

    // Initialize the semaphores. 
    sem_init(&numChairs, 0, n);

    // Create the barber.
    pthread_create()



    



}



