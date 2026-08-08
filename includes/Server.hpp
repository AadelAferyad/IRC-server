#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <unistd.h>
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
        void                        Initpoll();
        void                        acceptConnection();

        std::vector<struct pollfd>  pollFds;
        //dispatcher part
        Dispatcher                  dispatcher;
        void                        checkRegistration(Client& client);
        std::map<int, Client*>      clients;
        std::map<std::string, Channel> channels;
        
    public:
        Server(int _port, const std::string &_password);
        ~Server();

        void    run();

        // cmds handlers
        void Pass(Client&, const Command&);
        void Nick(Client&, const Command&);
        void User(Client&, const Command&);
        void Join(Client&, const Command&);
        void Part(Client&, const Command&);
        void Privmsg(Client&, const Command&);
        void Quit(Client&, const Command&);
};


#endif