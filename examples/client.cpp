#include <merlin.hpp>
#include <iostream>
#include <string>

int main() {
    Merlin::Client c("orangepizero");

    std::string msg;
    
    while(msg != "!exit") {
        getline(std::cin, msg);

        std::cout << c.send(msg) << std::endl;
    }

    return 0;
}