#include "Server.hpp"

void server::Part(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return;
    if (command.params.empty())
        return;

    std::string channelName = command.params[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    
    if (it == channels.end())
        return ;
    if (!it->second.hasClient(client.getFd()))
        return ;
    
    bool wasOP = it->second.isOperator(client.getFd());
    it->second.removeClient(client.getFd());
    if (it->second.getClients().empty())
    {
        channels.erase(it);
        return ;
    }
    if (wasOP)
        it->second.promoteNewOperator();
}
