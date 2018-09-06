#ifndef MERLIN_HOME_BRAIN_H
#define MERLIN_HOME_BRAIN_H

#include <string>
#include <microhttpd.h>

namespace Merlin {
    namespace Home {
        class Brain {
            void _startWebServer();
            int _answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls);
            int _on_client_connect(void *cls, const struct sockaddr *addr, socklen_t addrlen);
            int _save_get_data(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);
            bool _port_available(int portno);

            struct MHD_Daemon *_daemon;
            int _webServerPort;
            int _maxDevices;
            char _ip[128];
            char _name[128];
            char _role[128];

            public:
                Brain(int max_devices, int port = 80);
        };
    }
}

#endif