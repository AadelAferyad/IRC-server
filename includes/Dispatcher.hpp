#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <map>
#include <string>

class Server;
class Client;
struct Command;

class Dispatcher
{
    private:
        typedef void (Server::*CommandHandler)(Client&, const Command&);
        std::map<std::string, CommandHandler> routes;

    public:
        Dispatcher();
        ~Dispatcher();

        void dispatchCmd(Server& server, Client& client, const Command& command);
};

#endif