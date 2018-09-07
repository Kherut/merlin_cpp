#ifndef MERLIN_H
#define MERLIN_H

#include <string>

namespace Merlin {
    class Client {
        std::string _address;
        int _port;
        int _socket;
        int _buffer_size;

        int _get_port(std::string host, int port, std::string hostname, std::string ip, std::string role);
        std::string _get_hostname();
        std::string _get_ip();
        
        public:
            Client(std::string hostname, int = 256, int = 51000);
            ~Client();

            std::string send(std::string message);
            int port();
            std::string address();
    };

    class Server {
        std::string _address;
        int _port;
        int _socket, _socketalpha;
        int _buffer_size;
        
        int _get_port(std::string host, int port, std::string hostname, std::string ip, std::string role);
        std::string _get_hostname();
        std::string _get_ip();

        public:
            Server(std::string hostname, int = 256, int = 51000);
            ~Server();

            int receive(std::string destination, std::string = "RECV");
            int port();
            std::string address();
    };
}

#endif