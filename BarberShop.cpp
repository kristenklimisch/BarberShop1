#include <BarberShop.h>

void BarberShop::acquireChair(Customer c) {
    // sem chairs to be initialized to n
    sem_wait(&chairs);

}