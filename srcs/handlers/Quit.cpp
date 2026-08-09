#include "Server.hpp"

void server::Quit(Client &client, const Command &command)
{
    (void)command;
    int fd = client.getFd();
    std::map<std::string, Channel>::iterator it = channels.begin();

    while (it != channels.end())
    {
        if (it->second.hasClient(fd))
        {
            it->second.removeClient(fd);
            if (it->second.getClients().empty())
                channels.erase(it++);
            else
                ++it;
        }
        else
            ++it;
    }
    clients.erase(fd);
}
