#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <map>
#include <string>

class server;
class Client;
struct Command;

class Dispatcher
{
    private:
        typedef void (server::*CommandHandler)(Client&, const Command&);
        std::map<std::string, CommandHandler> routes;

    public:
        Dispatcher();
        ~Dispatcher();

        void dispatchCmd(server& server, Client& client, const Command& command);
};

#endif
