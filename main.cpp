#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CUSTOMERS 20

void *customer (void *num);
void *barber(void *);
// void inputCheck(int, int);

// Define mutex.
pthread_mutex_t m;

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
    
    int n, check, totalCustomers, i;

    // Accept user input for the number of chairs in the waiting room.
    printf("Enter the number of chairs in barbershop waiting room: ");
    check = scanf("%d", &n);
  //  inputCheck(check, n);

    // Accept user input for the number of customers.
    printf("Enter the number of customers. The maximum capacity of the babershop is %d customers: ", MAX_CUSTOMERS);
    check = scanf("%d", &totalCustomers);
 //   inputCheck(check, totalCustomers);

    // Verify that the user input for number of customers is within the maximum
    // capacity of the barbershop. If the user input exceeds the maximum capacity,
    // reset the number of customers to be equal to the maximum capacity.
    if (totalCustomers > MAX_CUSTOMERS) {
        printf("Error: The number of customers entered exceeds the maximum capacity\n");
        printf("of the barbershop. Resetting the number of customers to %d.\n", MAX_CUSTOMERS);
        totalCustomers = MAX_CUSTOMERS;
    }

    // Initialize the mutex.
    pthread_mutex_init(&m, NULL);

    // Initialize the semaphores. 
    sem_init(&chairsAvailable, 0, n);
    sem_init(&barberChair, 0, 0);
    sem_init(&wakeBarber, 0, 0); 

    // ID for barber thread.
    pthread_t btid;

    // Create the barber thread.
    pthread_create(&btid, NULL, barber, NULL);

    // IDs for customer threads.
    pthread_t tid[totalCustomers];
    
    // Create the customer threads.
    int customerID[totalCustomers];
    for (i = 0; i<totalCustomers; i++) {
        customerID[i] = i +1;
        pthread_create(&tid[i], NULL, customer, &customerID[i]);
        sleep(1); 
    }

    // Wait for all customer threads to finish.
    for (i = 0; i < totalCustomers; i++) {
        pthread_join(tid[i], 0);
    } 

    done_with_all_customers = 1;

    // Wake the barber so he will know he can exit. 
    sem_post(&wakeBarber); 

    pthread_join(btid, NULL);
}

void *barber(void *arg) {
    while(!(done_with_all_customers)) {

        
        // When there are no customers in the waiting room or
        // getting their hair cut, barber sleeps in the barber chair. 
        // KK - for some reason, this repeats every 4 customers.
        pthread_mutex_lock(&m);
        if (customersInShop == 0) {
            printf("Barber is sleeping in barber chair.\n");
        }
        pthread_mutex_unlock(&m);

        // Barber sleeps until he is woken up by a customer.
        sem_wait(&wakeBarber);

        // This only runs for very first customer to enter store. 
        printf("Barber is giving the barber chair to a customer.\n");
        printf("Barber is giving the customer a haircut.\n");

        // Sleep to simulate haircut. In this simple program,
        // all haircuts take the same amount of time. 
        // KK need to update this to generate random number.
        sleep(5);

        // Barber gives up barber chair.
        sem_post(&barberChair);
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
    pthread_mutex_lock(&m);
    customersInShop++;
    pthread_mutex_unlock(&m);

    // Wake up the barber.
    sem_post(&wakeBarber);
    
    // Wait until the barber chair is not occupied. 
    sem_wait(&barberChair);

    // Move from waiting room to work room and give up
    // chair in waiting room. 
    sem_post(&chairsAvailable);

    printf("Customer %d getting hair cut.\n", number);

    // After haircut is complete, customer gives up the barber chair.
    printf("Customer %d done with haircut and exiting barbershop.\n", number);

    // Decrement number of customers in barber shop when a customer
    // completes their haircut and exits barbershop.  
    pthread_mutex_lock(&m);
    customersInShop--;
    pthread_mutex_unlock(&m);
}




