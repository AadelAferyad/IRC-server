#include "Server.hpp"

void server::Invite(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return ;
    if (command.params.size() < 2)
        return ;

    std::string nickname = command.params[0];
    std::string channelName = command.params[1];
    std::map<std::string, Channel>::iterator ChannelIterator = channels.find(channelName);

    if (ChannelIterator == channels.end())
        return ;
    if (!ChannelIterator->second.hasClient(client.getFd()))
        return ;
    if (!ChannelIterator->second.isOperator(client.getFd()))
        return ;

    std::map<int, Client>::iterator ClientIterator = clients.begin();
    
    while (ClientIterator != clients.end())
    {
        if (ClientIterator->second.getNickname() == nickname)
            break ;
        ClientIterator++;
    }
    if (ClientIterator == clients.end())
        return ;
    if (ChannelIterator->second.hasClient(ClientIterator->second.getFd()))
        return ;
    ChannelIterator->second.addInvite(ClientIterator->second.getFd());
    //send reply 
}
