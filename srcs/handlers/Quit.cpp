#include "Server.hpp"

void server::Quit(Client &client, const Command &command)
{
    (void)command;
    int fd = client.getFd();
    std::map<std::string, Channel>::iterator it = channels.begin();

    while (it != channels.end())
    {
        Channel &channel = it->second;
        if (channel.hasClient(fd))
        {
            bool wasOperator = channel.isOperator(fd);
            channel.removeClient(fd);
            if (channel.getClients().empty())
            {
                std::map<std::string, Channel>::iterator toErase = it;
                it++;
                channels.erase(toErase);
                continue;
            }
            if (wasOperator)
                channel.promoteNewOperator();
        }
        it++;
    }
    clients.erase(fd);
}
