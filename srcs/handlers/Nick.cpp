#include "../includes/Server.hpp"

bool Server::nicknameExists(const std::string& nickname)
{
    std::map<int, Client*>::iterator it;

    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second->getNickname() == nickname)
            return true;
    }
    return false;
}

void Server::Nick(Client &client, const Command &command)
{
    if (command.params.empty())
        return ;
    if (nicknameExists(command.params[0]))
        return ;
    client.setNickname(params[0]);
    client.setNickReceived(true);

    checkRegistration(client);
}
