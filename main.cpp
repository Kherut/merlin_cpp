#include "merlin/home/device.hpp"
#include <iostream>
#include <string>

int main() {
    Merlin::Home::Device::Client c("arch_hpcompaq");

    std::string msg;
    
    while(msg != "!exit") {
        getline(std::cin, msg);

        std::cout << c.send(msg) << std::endl;
    }

    return 0;
}