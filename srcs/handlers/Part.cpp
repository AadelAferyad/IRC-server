#include "Server.hpp"

void Part(Client &client, const Command &command)
{
    if (!client.registered())
        return;
    if (command.params.empty())
        return;

    std::string channelName = command.params[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    
    if (it == channels.end())
        return ;
    if (!it.second.hasClient(client.getFd()))
        return ;
    
    it->second.removeClient(client.getFd());
    if (it->second.getClients().empty())
        channels.erase(it);
}
