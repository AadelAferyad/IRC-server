#include "../includes/Dispatcher.hpp"
#include "../includes/Server.hpp"

Dispatcher::Dispatcher()
{
    routes["PASS"]    = &Server::Pass;
    routes["NICK"]    = &Server::Nick;
    routes["USER"]    = &Server::User;
    routes["JOIN"]    = &Server::Join;
    routes["PART"]    = &Server::Part;
    routes["PRIVMSG"] = &Server::Privmsg;
    routes["QUIT"]    = &Server::Quit;
}

Dispatcher::~Dispatcher() {}

void Dispatcher::dispatchCmd(Server& server, Client& client, const Command& command)
{
    std::map<std::string, CommandHandler>::iterator it;
    it = routes.find(command.cmd_name);

    if (it == routes.end())
        return;

    (server.*(it->second))(client, command);
}