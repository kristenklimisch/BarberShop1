#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CUSTOMERS 25

void *customer (void *num);
void *barber(void *);

// Define the semaphores.

// chairsAvailable tracks number of chairs available
// in the waiting room and ensures that each chair 
// is only accessed by 1 customer at a time.
sem_t chairsAvailable;

// barberChair ensures mutual exclusion of the barber's 
// chair. 
sem_t barberChair;

// wakeBarber allows the barber to sleep until
// a customer wakes him. 
sem_t wakeBarber;

// Flag indicating barber's work is done for the day. 
bool done_with_all_customers = FALSE; 

void 

int main (int argc, char *argv[]) {
    int numCustomers = NUM_CUSTOMERS;
    
    // ID for barber thread.
    pthread_t btid;

    // IDs for customer threads.
    pthread_t tid[numCustomers];

    int n, check, i;

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
    sem_init(&chairsAvailable, 0, n);
    sem_init(&barberChair, 0, 1);
    sem_init(&wakeBarber, 0, 1);

    // Create the barber thread.
    pthread_create(&btid, NULL, barber, NULL);

    // Create the customer threads.
    int customerID[numCustomers];
    for (i = 0; i<numCustomers; i++) {
        customer_ID[i] = i;
        pthread_create(&tid[i], NULL, customer, &customerID[i]);
        sleep(1); 
    }

    // Wait for all customer threads to finish.
    for (i = 0; i < numCustomers; i++) {
        pthread_join(tid[i], 0);
    } 

    done_with_all_customers = TRUE;

    // Wake the barber so he will know he can exit. 
    sem_post(&wakeBarber); 

    pthread_join(btid, NULL);
}

void *barber(void *arg) {

}

void *customer(void *customerNumber) {
    int number = *(int *)customerNumber;
    printf("Customer %d attempting to enter barber shop\n");
    
    // Wait until a chair in the waiting room 
    // is available. 
    sem_wait(&chairsAvailable);
    printf("Customer %d entering waiting room and sitting in chair");

    // Wait until the barber chair is free.
    sem_wait(&barberChair);

    // When the barber chair is free, give up
    // your chair in the waiting room.
    sem_post(&chairsAvailable);

    // KK note- how are we waking the barber when the barber is asleep?
    // Wake up the barber
    






}




