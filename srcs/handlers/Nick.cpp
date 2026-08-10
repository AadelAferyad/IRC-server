#include "Server.hpp"

bool server::nicknameExists(const std::string& nickname)
{
    std::map<int, Client>::iterator it;

    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second.getNickname() == nickname)
            return true;
    }
    return false;
}

void server::Nick(Client &client, const Command &command)
{
    if (command.params.empty())
    {
        sendNumeric(client, ERR_NONICKNAMEGIVEN, client.getNickname(), "No nickname given");
        return ;
    }

    if (nicknameExists(command.params[0]))
    {
        sendNumeric(client, ERR_NICKNAMEINUSE, client.getNickname() + " " + command.params[0], "Nickname is already in use");
        return ;
    }

    client.setNickname(command.params[0]);
    client.setNickReceived(true);

    checkRegistration(client);
}