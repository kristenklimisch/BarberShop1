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
    sem_init(&wakeBarber, 0, 0); //I think this makes sense as a 0, 
    // sample code did it as 1 though. 
    // 3 easy peices says all locks should be initialized to 1.
    // Note for KK: if errors, can look at starting values of sems

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
    while(!(done_with_all_customers)){
        printf("Barber is sleeping.\n");

        // Barber sleeps until he is woken up by a customer.
        sem_wait(&wakeBarber);
        // Barber surrenders barber chair. 
        sem_post(&barberChair);

        printf("Barber is giving up his barber chair to give a haircut.\n");


        sleep(5);



    }
}

void *customer(void *customerNumber) {
    int number = *(int *)customerNumber;
    printf("Customer %d attempting to enter barber shop.\n");
    
    // Wait until a chair in the waiting room 
    // is available. 
    sem_wait(&chairsAvailable);
    printf("Customer %d entering waiting room and sitting in chair.\n");

    // Wait until the barber chair is free.
    sem_wait(&barberChair);

    // When the barber chair is free, customer gives up 
    // their chair in the waiting room.
    sem_post(&chairsAvailable);

    // Wake up the barber.
    sem_post(&wakeBarber);

    printf("Customer %d getting hair cut.\n");

    // After finishing haircut, give up the barber chair.
    sem_post(&barberChair);
    printf("Customer %d done with haircut and exiting barbershop.\n");
}



    






}




