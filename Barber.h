#include <thread>
#include <semaphore>

class Barber {
    cond_t noCustomers;
    mutex_t mutex;
    
    void cutHair();

    void wake();



};