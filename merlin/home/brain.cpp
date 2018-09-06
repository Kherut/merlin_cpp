#include "brain.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <strings.h>
#include <microhttpd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace Merlin {
    namespace Home {
        Brain::Brain(int max_devices, int port) {
            _webServerPort = port;

            if(max_devices == 0)
                max_devices = 65536 - _webServerPort;

            _maxDevices = max_devices;

            _startWebServer();

            std::cout << "Created Brain!" << std::endl;
        }

        int Brain::_save_get_data (void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
            if(strcmp(key, "ip") == 0)
                strcpy(_ip, value);

            else if(strcmp(key, "name") == 0)
                strcpy(_name, value);

            else if(strcmp(key, "role") == 0)
                strcpy(_role, value);

            return MHD_YES;
        }

        int Brain::_on_client_connect (void *cls, const struct sockaddr *addr, socklen_t addrlen) {
            //struct sockaddr_in *sin = (struct sockaddr_in *) addr;
            //char* ip = inet_ntoa(sin->sin_addr);
            //printf("%s\n", ip);

            return MHD_YES;
        }

        bool Brain::_port_available(int portno) {
            int sockfd;
            socklen_t clilen;
            struct sockaddr_in serv_addr, cli_addr;
            bool available = true;

            sockfd = socket(AF_INET, SOCK_STREAM, 0);

            if (sockfd < 0)
                std::cout << "Ouch! We've had a trouble opening the socket." << std::endl;

            bzero((char *) &serv_addr, sizeof(serv_addr));

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);

            if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                available = false;

            close(sockfd);

            return available;
        }

        int Brain::_answer_to_connection (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
            MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, _save_get_data, NULL);    

            char *page = (char*) malloc(sizeof(char) * 512);
            char port_string[7];

            bzero(port_string, 7);

            int i = _webServerPort;

            while(i < _webServerPort + _maxDevices && !_port_available(i))
                i++;

            snprintf(port_string, 7, "%d", i);

            strcpy(page, port_string);

            //OPEN EITHER CLIENT OR SERVER
            char* clientCmd = (char*) malloc(sizeof(char) * 256);
            char* serverCmd = (char*) malloc(sizeof(char) * 256);

            //SEPARATE THREADS?

            if(strcmp(_role, "CLIENT") == 0) {
                popen(serverCmd, "r");
                printf("Opened server on port %s.\n", port_string);
            }

            else if(strcmp(_role, "SERVER") == 0) {
                popen(clientCmd, "r");
                printf("Opened client on port %s.\n", port_string);
            }

            struct MHD_Response *response;
            int ret;

            response = MHD_create_response_from_buffer (strlen (page), (void*) page, MHD_RESPMEM_PERSISTENT);

            ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
            MHD_destroy_response (response);

            return ret;
        }

        void Brain::_startWebServer() {
            _daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, _webServerPort, _on_client_connect, NULL, &_answer_to_connection, NULL, MHD_OPTION_END);

            std::cout << "Started web server on port " << _webServerPort << "!" << std::endl;
        }
    }
}