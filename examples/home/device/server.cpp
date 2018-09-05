#include "merlin/home/device.hpp"
#include <iostream>
#include <string>

int main() {
    Merlin::Home::Device::Server s("arch_hpcompaq");

    std::string msg;
    int code = 0;
    
    while(msg != "!exit" && code >= 0) {
        code = s.receive(msg);
    }

    return 0;
}