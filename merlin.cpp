#include <merlin.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <algorithm>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>

namespace Merlin {
    //CLIENT - CONSTRUCTOR AND DESTRUCTOR
    Client::Client(std::string hostname, int buffer_size, int port) {
        _address = hostname;
        _buffer_size = buffer_size;
        _port = _get_port(hostname, port, _get_hostname(), _get_ip(), "CLIENT");

        struct sockaddr_in serv_addr;
        struct hostent *server;
        
        _socket = socket(AF_INET, SOCK_STREAM, 0);

        if (_socket < 0)
            std::cerr << "There was a problem while opening a socket :(" << std::endl;

        server = gethostbyname(_address.c_str());

        if (server == NULL)
            std::cerr << "Apparently there's no such host :^(" << std::endl;

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(_port);

        //GIVE MHB TIME TO OPEN SERVER
        usleep(750 * 1000);

        if (connect(_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            std::cerr << "Beep boop, we couldn't connect to given host :(" << std::endl;
    }

    Client::~Client() {
        close(_socket);
        std::cout << "Destroyed client" << std::endl;
    }

    //CLIENT - PUBLIC
    std::string Client::send(std::string message) {
        int code;
        char buffer[_buffer_size];

        bzero(buffer, _buffer_size);

        if(message.length() > _buffer_size) {
            code = write(_socket, message.substr(0, _buffer_size).c_str(), _buffer_size);
            message = message.substr(_buffer_size, message.length());
            send(message);
        }

        else
            code = write(_socket, message.substr(0, message.length()).c_str(), message.length());

        if(code < 0)
            std::cerr << "Couldn't write to socket!" << std::endl;

        code = read(_socket, buffer, _buffer_size);

        if(code < 0)
            std::cerr << "Couldn't read from socket!" << std::endl;

        return std::string(buffer);
    }

    int Client::port() {
        return _port;
    }

    std::string Client::address() {
        return _address;
    }

    //CLIENT - PRIVATE
    std::string Client::_get_hostname() {
        char *hostname = (char*) malloc(64 * sizeof(char));
        gethostname(hostname, 64);

        return std::string(hostname);
    }

    std::string Client::_get_ip() {
        FILE *f;
        char line[100] , *p , *c;

        f = fopen("/proc/net/route" , "r");

        while(fgets(line , 100 , f))
        {
            p = strtok(line , " \t");
            c = strtok(NULL , " \t");

            if(p!=NULL && c!=NULL)
            {
                if(strcmp(c , "00000000") == 0)
                {
                    //printf("Default interface is : %s \n" , p);
                    break;
                }
            }
        }

        //which family do we require , AF_INET or AF_INET6
        int fm = AF_INET;
        struct ifaddrs *ifaddr, *ifa;
        int family , s;
        char host[NI_MAXHOST];

        char *ret = (char*) malloc(NI_MAXHOST * sizeof(char));

        if (getifaddrs(&ifaddr) == -1)
            std::cerr << "We couldn't retrieve your IP :o" << std::endl;

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
            {
                continue;
            }

            family = ifa->ifa_addr->sa_family;

            if(strcmp( ifa->ifa_name , p) == 0)
            {
                if (family == fm)
                {
                    s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);

                    if (s != 0)
                        std::cerr << "Function called 'getnameinfo' didn't work as it was supposed to, but I don't know why :/" << std::endl;

                    sprintf(ret, "%s", host);
                }
            }
        }

        freeifaddrs(ifaddr);

        return ret;
    }

    int Client::_get_port(std::string host, int port, std::string hostname, std::string ip, std::string role) {
        curlpp::Cleanup cu;
        std::ostringstream os;

        os << curlpp::options::Url(std::string("http://" + host + ":" + std::to_string(port) + "/control/devices/new/" + ip + "/" + hostname + "/" + role));

        return atoi(os.str().c_str());
    }

    //-----

    //SERVER - CONSTRUCTOR AND DESTRUCTOR
    Server::Server(std::string hostname, int buffer_size, int port) {
        _address = hostname;
        _port = _get_port(hostname, port, _get_hostname(), _get_ip(), "SERVER");

        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int code;

        _socketalpha = socket(AF_INET, SOCK_STREAM, 0);

        if (_socketalpha < 0)
            std::cerr << "There was a problem while opening the socket :(" << std::endl;

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(_port);

        if (bind(_socketalpha, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            std::cerr << "There was a problem while binding to the socket :(" << std::endl;

        listen(_socketalpha, 5);

        clilen = sizeof(cli_addr);
        _socket = accept(_socketalpha, (struct sockaddr *) &cli_addr, &clilen);

        if (_socket < 0)
            std::cerr << "Looks like the other side of the connection didn't want to connect after all!" << std::endl;
    }

    Server::~Server() {
        close(_socket);
        close(_socketalpha);
        std::cout << "Destroyed server" << std::endl;
    }

    //SERVER - PUBLIC
    int Server::receive(std::string destination, std::string response) {
        char buffer[_buffer_size];
        bzero(buffer, _buffer_size);

        int code = read(_socket, buffer, _buffer_size - 1);

        if(code < 0) {
            std::cerr << "There was an error while reading from socket :(" << std::endl;

            return code;
        }

        destination = std::string(buffer);

        return code;
    }

    int Server::port() {
        return _port;
    }

    std::string Server::address() {
        return _address;
    }

    //SERVER - PRIVATE
    std::string Server::_get_hostname() {
        char *hostname = (char*) malloc(64 * sizeof(char));
        gethostname(hostname, 64);

        return std::string(hostname);
    }

    std::string Server::_get_ip() {
        FILE *f;
        char line[100] , *p , *c;

        f = fopen("/proc/net/route" , "r");

        while(fgets(line , 100 , f))
        {
            p = strtok(line , " \t");
            c = strtok(NULL , " \t");

            if(p!=NULL && c!=NULL)
            {
                if(strcmp(c , "00000000") == 0)
                {
                    //printf("Default interface is : %s \n" , p);
                    break;
                }
            }
        }

        //which family do we require , AF_INET or AF_INET6
        int fm = AF_INET;
        struct ifaddrs *ifaddr, *ifa;
        int family , s;
        char host[NI_MAXHOST];

        char *ret = (char*) malloc(NI_MAXHOST * sizeof(char));

        if (getifaddrs(&ifaddr) == -1)
            std::cerr << "We couldn't retrieve your IP :o" << std::endl;

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
            {
                continue;
            }

            family = ifa->ifa_addr->sa_family;

            if(strcmp( ifa->ifa_name , p) == 0)
            {
                if (family == fm)
                {
                    s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);

                    if (s != 0)
                        std::cerr << "Function called 'getnameinfo' didn't work as it was supposed to, but I don't know why :/" << std::endl;

                    sprintf(ret, "%s", host);
                }
            }
        }

        freeifaddrs(ifaddr);

        return ret;
    }

    int Server::_get_port(std::string host, int port, std::string hostname, std::string ip, std::string role) {
        curlpp::Cleanup cu;
        std::ostringstream os;

        os << curlpp::options::Url(std::string("http://" + host + ":" + std::to_string(port) + "/control/devices/new/" + ip + "/" + hostname + "/" + role));

        return atoi(os.str().c_str());
    }
}
