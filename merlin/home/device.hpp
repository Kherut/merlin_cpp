#ifndef MERLIN_HOME_DEVICE_H
#define MERLIN_HOME_DEVICE_H

#include <string>

namespace Merlin {
    namespace Home {
        namespace Device {
            class Client {
                std::string _address;
                int _port;
                int _socket;
                int _buffer_size;

                int _get_port(std::string host, int port, std::string hostname, std::string ip, std::string role);
                std::string _get_hostname();
                std::string _get_ip();
                
                public:
                    Client(std::string, int = 256, int = 51000);
                    ~Client();

                    std::string send(std::string message);
                    int port();
                    std::string address();
            };

            class Server {
                std::string address;
                
                public:
                    Server(std::string hostname);
                    ~Server();
            };
        }
    }
}

#endif