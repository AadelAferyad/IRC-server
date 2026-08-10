#include "Server.hpp"

void server::Mode(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return ;
    if (command.params.size() < 2)
        return ;

    std::string channelName = command.params[0];
    std::string mode = command.params[1];
    std::map<std::string, Channel>::iterator channelIt;
    channelIt = channels.find(channelName);
    if (channelIt == channels.end())
        return ;
    if (!channelIt->second.hasClient(client.getFd()))
        return ;
    if (!channelIt->second.isOperator(client.getFd()))
        return ;
    if ((mode == "+o" || mode == "-o") && command.params.size() < 3)
        return ;
    if (mode != "+o" && mode != "-o")
        return ;

    std::string nickname = command.params[2];
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
    if (mode == "+o")
        channelIt->second.addOperator(targetFd);
    else if (mode == "-o")
        channelIt->second.removeOperator(targetFd);

    // reply
}