#include "Server.hpp"

void server::Kick(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return ;
    if (command.params.size() < 2)
        return ;

    std::string channelName = command.params[0];
    std::string nickname = command.params[1];
    std::map<std::string, Channel>::iterator channelIt = channels.find(channelName);

    if (channelIt == channels.end())
    {
        sendNumeric(client, ERR_NOSUCHCHANNEL,
            client.getNickname() + " " + channelName,
            "No such channel");
        return ;
    }
    if (!channelIt->second.hasClient(client.getFd()))
    {
        sendNumeric(client, ERR_NOTONCHANNEL,
            client.getNickname() + " " + channelName,
            "You're not on that channel");
        return ;
    }
    if (!channelIt->second.isOperator(client.getFd()))
    {
        sendNumeric(client, ERR_CHANOPRIVSNEEDED,
            client.getNickname() + " " + channelName,
            "You're not channel operator");
        return ;
    }

    std::map<int, Client>::iterator clientIt = clients.begin();
    while (clientIt != clients.end())
    {
        if (clientIt->second.getNickname() == nickname)
            break ;

        ++clientIt;
    }
    if (clientIt == clients.end())
        return ;
    int targetFd = clientIt->second.getFd();
    if (!channelIt->second.hasClient(targetFd))
        return ;

    bool wasOperator = channelIt->second.isOperator(targetFd);
    channelIt->second.removeClient(targetFd);
    if (channelIt->second.getClients().empty())
    {
        channels.erase(channelIt);
        return ;
    }
    if (wasOperator)
        channelIt->second.promoteNewOperator();
}