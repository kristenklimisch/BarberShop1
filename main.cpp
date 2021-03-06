#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CUSTOMERS 20
#define MIN_SLEEP 1
#define MAX_SLEEP 10

void *customer (void *num);
void *barber(void *);
void inputCheck(int, int);
void randSleep();

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

// Global variable for number of customers in the waiting room.
int customersInWaitingRoom = 0;

int main (int argc, char *argv[]) {
    
    int n, check, totalCustomers, i;

    // Seeding random sleep function. 
    srand(time(0));

    // Accept user input for the number of chairs in the waiting room.
    printf("Enter the number of chairs in barbershop waiting room: ");
    check = scanf("%d", &n);
    inputCheck(check, n);

    // Accept user input for the number of customers.
    printf("Enter the number of customers. The maximum capacity of the babershop is %d customers: ", MAX_CUSTOMERS);
    check = scanf("%d", &totalCustomers);
    inputCheck(check, totalCustomers);

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

        // Sleep to simulate "natural" flow of customers. 
        randSleep(); 
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
    printf("Barber sleeps while waiting customers to arrive.\n");
    sem_wait(&wakeBarber);
    while(!(done_with_all_customers)) {
            printf("Barber is giving the customer a haircut.\n");

            // Sleep to simulate haircut. 
            randSleep();

            printf("Barber completes haircut. \n");
            // Barber gives up barber chair.
            sem_post(&barberChair);

            pthread_mutex_lock(&m);
            if (customersInWaitingRoom == 0) {
                printf("No customers in waiting room. Barber goes to sleep.\n");
            }
            pthread_mutex_unlock(&m);

            // Wait for next customer or for main thread. 
            sem_wait(&wakeBarber);
    }
    printf("Barber is going home for the day.\n");
}

void *customer(void *customerNumber) {
    int number = *(int *)customerNumber;
    printf("Customer %d arriving at barber shop.\n", number);

    // Wait until a chair is available in the waiting room 
    // to enter the waiting room. 
    sem_wait(&chairsAvailable);

    // Increment number of customers in barber shop when a customer
    // enters the waiting room.
    pthread_mutex_lock(&m);
    // If we wanted to kick customers out if the waiting room was full when they 
    // arrived at the barber shop, we would add a conditional here to compare
    // the number of customers in the waiting room with the chairs available. 
    // We'd also delete the blocking chairsAvailable semaphore. 
    customersInWaitingRoom++;
    printf("Customer %d entering waiting room. There are now %d customers in the waiting room.\n", number, customersInWaitingRoom);
    pthread_mutex_unlock(&m);

    // Wake up the barber.
    // Rename customer ready
    sem_post(&wakeBarber);

    // Move from waiting room to work room and give up
    // chair in waiting room. 
    sem_post(&chairsAvailable);

    // Decrement number of customers in barber shop when a customer
    // completes their haircut and exits barbershop.  
    pthread_mutex_lock(&m);
    customersInWaitingRoom--;
    pthread_mutex_unlock(&m);

    // Wait until the barber completes haircut.
    printf("Customer %d getting hair cut.\n", number); 
    sem_wait(&barberChair);
    
    // After haircut is complete, customer gives up the barber chair.
    printf("Customer %d has exited.\n", number);
}

// Function to verify that user input meets program criteria. 
// Parameters: check - the value returned from using the scanf function to read user input. 
//             n - the value entered by the user. 
void inputCheck(int check, int n) {
    if ((check < 1) || (n < 0)) {
        printf("Invalid entry - entry must be a positive number.\n");
        printf("Program exiting.");
        exit(-1);
    }    

}

void randSleep() {
    int randNum = rand()%(MAX_SLEEP - MIN_SLEEP + 1) + MIN_SLEEP;
    sleep(randNum);
}




