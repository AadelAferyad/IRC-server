#include "../../includes/Server.hpp"

void Server::Join(Client& client, const Command& command)
{
    if (!client.isRegistered())
        return;
    if (command.params.empty())
        return;

    std::string channelName = command.params[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end())
    {
        channels.insert(std::make_pair(channelName, Channel(channelName)));
        it = channels.find(channelName);
    }

    if (it->second.hasClient(client.getFd()))
        return;
    it->second.addClient(client.getFd());
}