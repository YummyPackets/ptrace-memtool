#include <iostream>
#include <unistd.h>


int main(){
    int var1 = 1; // Set variable to be watched
    int var2 = 2; // Set another variable to be watched
    int* ptr1 = &var1; // Create pointer to var1 to allow watching its address and value
    int* ptr2 = &var2; // Create pointer to var2 to allow watching its address and value
    int id = getpid(); // Get the process ID of the current program to allow the watcher program to attach to it
    while (true){ // Infinite loop to keep the program running so that the watcher can continuously monitor the variables
        std::cout<< "Programs PID: " << id << " | " <<"Current variables: "<< *ptr1 << " , " << *ptr2<< " | Addresses: " << &ptr1 << " , " << &ptr2<<"\n";
        sleep(2); // sleep for 2 seconds to slow down the output of the program.
    }
}