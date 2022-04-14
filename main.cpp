#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CUSTOMERS 25

void *customer (void *num);
void *barber(void *);


// Define mutex.
pthread_mutex_t customers;

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

// Flag used to indicate when the barber's work is done for the day.
// Initialize to 0. When value changes to 1, barber is done for the
// day. 
bool done_with_all_customers = 0; 

// Global variable for number of customers in the barbershop.
int customersInShop = 0;

int main (int argc, char *argv[]) {
    int numCustomers = NUM_CUSTOMERS;
    
    // ID for barber thread.
    pthread_t btid;

    // IDs for customer threads.
    pthread_t tid[numCustomers];

    int n, check, i;

    // Accept user input for the number of chairs in the waiting room.
    printf("Enter the number of chairs in barbershop waiting room: ");
    check = scanf("%d", &n);
    printf("n is %d", n);

    // Perform error checking for user input. 
   // if ((check != 1) || (n < 0)) {
    if (n < 0) {
        printf("Invalid entry - number of chairs must be a positive number.\n");
        printf("Program exiting.\n");
        exit(-1);
    }

    // Initialize the mutex.
    pthread_mutex_init(&customers, NULL);

    // Initialize the semaphores. 
    sem_init(&chairsAvailable, 0, n);
    sem_init(&barberChair, 0, 1);
    sem_init(&wakeBarber, 0, 1); 

    // Create the barber thread.
    pthread_create(&btid, NULL, barber, NULL);

    // Create the customer threads.
    int customerID[numCustomers];
    for (i = 0; i<numCustomers; i++) {
        customerID[i] = i +1;
        pthread_create(&tid[i], NULL, customer, &customerID[i]);
        sleep(1); 
    }

    // Wait for all customer threads to finish.
    for (i = 0; i < numCustomers; i++) {
        pthread_join(tid[i], 0);
    } 

    done_with_all_customers = 1;

    // Wake the barber so he will know he can exit. 
    sem_post(&wakeBarber); 

    pthread_join(btid, NULL);

    printf("Barber is leaving for the day.");
}

void *barber(void *arg) {
    while(!(done_with_all_customers)){
        
        // When there are no customers in the waiting room or
        // getting their hair cut, barber sleeps in the barber chair. 
        // KK - for some reason, this repeats every 4 customers.

        pthread_mutex_lock(&customers);
        if (customersInShop == 0) {
            printf("Barber is sleeping in barber chair.\n");
        }
        pthread_mutex_unlock(&customers);

        
        // Barber sleeps until he is woken up by a customer.
        sem_wait(&wakeBarber);

        // Barber gives up barber chair.
        sem_post(&barberChair);

        printf("Barber is giving the barber chair to a customer.\n");
        printf("Barber is giving the customer a haircut.\n");

        // Sleep to simulate haircut. In this simple program,
        // all haircuts take the same amount of time. 
        // KK need to update this to generate random number.
        sleep(5);

    }
}

void *customer(void *customerNumber) {
    int number = *(int *)customerNumber;
    printf("Customer %d attempting to enter barber shop.\n", number);

    // Wait until a chair is available in the waiting room 
    // to enter the waiting room. 
    sem_wait(&chairsAvailable);
    printf("Customer %d entering waiting room.\n", number);

    // Increment number of customers in barber shop when a customer
    // enters the waiting room. 
    pthread_mutex_lock(&customers);
    customersInShop++;
    pthread_mutex_unlock(&customers);

    // Wake up the barber.
    sem_post(&wakeBarber);
    
    // Wait until the barber chair is not occupied. 
    sem_wait(&barberChair);

    // Move from waiting room to work room and give up
    // chair in waiting room. 
    sem_post(&chairsAvailable);

    printf("Customer %d getting hair cut.\n", number);

    // After haircut is complete, customer gives up the barber chair.
    sem_post(&barberChair);
    printf("Customer %d done with haircut and exiting barbershop.\n", number);

    // Decrement number of customers in barber shop when a customer
    // completes their haricut and exits barbershop.  
    pthread_mutex_lock(&customers);
    customersInShop--;
    pthread_mutex_unlock(&customers);
}




