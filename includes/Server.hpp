#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>
#include <stdexcept>

class Client;

class Server
{
    private:
        int                         port;
        std::string                 password;
        int                         listenFD;
        void                        SetupListeningSocket();
        std::vector<struct pollfd>  pollFds;
        std::map<int, Client*>      clients;

    public:
        Server(int _port, const std::string &_password);
        ~Server();

        void    run();
};


#endif