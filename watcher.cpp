#include <sys/ptrace.h>     // ptrace
#include <sys/wait.h>       // waitpid
#include <sys/types.h>      // pid_t
#include <unistd.h>         // getpid, sleep
#include <iostream>          // cout, cin
#include <cstdlib>           // atoi
#include <cstdint>           // uint64_t
#include <cerrno>            // errno
#include <cstring>           // strerror
#include <limits>
#include <string>

int main(int arc, char* argv[]){
    pid_t pid= atoi(argv[1]);
    //uint64_t address = strtoull(argv[2], nullptr, 16);
    int status;
    int choice;
    std::cout << "Target PID: "<< pid << "\n";

    //attach to given program using PID
    ptrace(PTRACE_ATTACH, pid, nullptr, nullptr);
    waitpid(pid, &status, 0); // wait for program to freeze before continuing to ensure data doesnt change.
    std::cout << "Attached to: " << pid << "\n";

    while (true){
        std::cout << "\n1. Read value at address\n";
        std::cout << "2. Write a new value to the address\n";
        std::cout << "3. Read with an offset\n";
        std::cout << "4. Display value as different types\n";
        std::cout << "5. Quit\n";
        std::cout << "Choice? ";
        std::cin >> std::dec >> choice;

        switch(choice){
            case 1: {
                uint64_t address_case_1;
                std::cout << "Address: ";
                std::cin >> std::hex >> address_case_1;
                errno = 0;
                int case_1_value = ptrace(PTRACE_PEEKDATA, pid, (void*)address_case_1, nullptr);
                if (errno !=0){
                std::cout << "Failed to read address" << "\n";
                }
                std::cout <<"Found data: " << case_1_value << "\n"; 
                break;
            }
            case 2: {
                int case_2_new_value; // Set variable for new value input
                uint64_t address_case_2; // Set variable for address input
                std::cout << "Address: ";
                std::cin >> std::hex >> address_case_2;
                errno = 0; // Clear errno before reading
                long case_2_value = ptrace(PTRACE_PEEKDATA, pid, (void*)address_case_2, nullptr); // Read current value at address
                std::cout << "Value to write: ";
                std::cin >> case_2_new_value;
                long masked = (case_2_value & 0xFFFFFFFF00000000) | (uint32_t)case_2_new_value; // Combine new value with existing upper bits
                if (ptrace(PTRACE_POKEDATA, pid, (void*)address_case_2, (void*)masked) == -1) { // Write new value to address
                std::cerr << "POKE failed: " << strerror(errno) << "\n"; // Check for errors
                }
                break;
            }
            case 3: {
                uint64_t base_address; // Set variable for base address input
                int offset; // Set variable for offset input
                std::cout << "Base Address: ";
                std::cin >> std::hex >> base_address; // Read base address in hexadecimal
                std::cout << "Offset (in bytes): ";
                std::cin >> std::dec >> offset;
                uint64_t target_address = base_address + offset; // Calculate target address by adding offset to base address
                errno = 0;  // Clear errno before reading
                int offset_value = ptrace(PTRACE_PEEKDATA, pid, (void*)target_address, nullptr); // Read value at target address
                if (errno != 0) { // Check for errors
                    std::cout << "Failed to read address with offset\n"; // If there was an error, print a message
                } else {
                    std::cout << "Value at address " << std::hex << target_address << ": " << std::dec << offset_value << "\n"; // If the read was successful, print the value at the target address
                }
                break;
            }
            case 4: {
                uint64_t address; // Set variable for address input
                std::cout << "Address: ";
                std::cin >> std::hex >> address; // Read address in hexadecimal
                errno = 0; // Clear errno before reading
                int value = ptrace(PTRACE_PEEKDATA, pid, (void*)address, nullptr); // Read value at address
                if (errno != 0) { // Check for errors
                    std::cout << "Failed to read address\n";
                } else {
                    std::cout << "Raw value at address " << std::hex << address << ": " << std::dec << value << "\n"; // Print raw value at address
                    std::cout << "Interpreted as int: " << *(int*)&value << "\n"; // Interpret value as int and print
                    std::cout << "Interpreted as char: " << *(char*)&value << "\n"; // Interpret value as char and print
                    std::cout << "Interpreted as float: " << *(float*)&value << "\n"; // Interpret value as float and print
                }
                break;
            }
            case 5: {
                ptrace(PTRACE_DETACH, pid, nullptr, nullptr); // Detach from the target process
                std::cout << "Detached.\n";
                return 0;
            }
            default: {
                std::cout << "Invalid choice\n"; // If the user enters an invalid choice, print a message
            }
        }
    }

}