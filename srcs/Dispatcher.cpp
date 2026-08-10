#include "Dispatcher.hpp"
#include "Server.hpp"

Dispatcher::Dispatcher()
{
    routes["PASS"]    = &server::Pass;
    routes["NICK"]    = &server::Nick;
    routes["USER"]    = &server::User;
    routes["JOIN"]    = &server::Join;
    routes["PART"]    = &server::Part;
    routes["PRIVMSG"] = &server::Privmsg;
    routes["QUIT"]    = &server::Quit;
    routes["TOPIC"]   = &server::Topic;
    routes["INVITE"]  = &server::Invite;
    routes["KICK"]    = &server::Kick;
    routes["MODE"]    = &server::Mode;
}

Dispatcher::~Dispatcher() {}

void Dispatcher::dispatchCmd(server& server, Client& client, const Command& command)
{
    std::map<std::string, CommandHandler>::iterator it;
    it = routes.find(command.cmd_name);

    if (it == routes.end())
        return;

    (server.*(it->second))(client, command);
}
