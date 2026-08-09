#include "Server.hpp"

void server::Topic(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return ;
    if (command.params.empty())
        return ;

    std::string channelName = command.params[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end())
        return ;
    if (!it->second.hasClient(client.getFd()))
        return ;

    if (command.params.size() == 1)
    {
        // broadcast current topic back to client
        return ;
    }
    std::string topic = command.params[1];
    it->second.setTopic(topic);
    // broadcast changed topic to all channel's members
}
