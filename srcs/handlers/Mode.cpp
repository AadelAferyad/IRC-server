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
    // +i & -i
    if (mode == "+i")
    {
        channelIt->second.setInviteOnly(true);
        return ;
    }
    if (mode == "-i")
    {
        channelIt->second.setInviteOnly(false);
        return ;
    }
    // +t & -t
    if (mode == "+t")
    {
        channelIt->second.setTopicRestricted(true);
        return ;
    }

    if (mode == "-t")
    {
        channelIt->second.setTopicRestricted(false);
        return ;
    }
    // +k & -k
    if (mode == "+k")
    {
        if (command.params.size() < 3)
            return ;

        std::string key = command.params[2];

        if (key.empty())
            return ;

        channelIt->second.setKey(key);
        return ;
    }
    if (mode == "-k")
    {
        channelIt->second.removeKey();
        return ;
    }
    // +l & -l
    if (mode == "+l")
    {
        if (command.params.size() < 3)
            return ;
        int limit = std::atoi(command.params[2].c_str());
        if (limit <= 0)
            return ;
        channelIt->second.setUserLimit(limit);
        return ;
    }
    if (mode == "-l")
    {
        channelIt->second.removeUserLimit();
        return ;
    }
    // +o & -o
    if (mode != "+o" && mode != "-o")
        return ;
    if (command.params.size() < 3)
        return ;

    std::string nickname = command.params[2];
    std::map<int, Client>::iterator clientIterator = clients.begin();
    while (clientIterator != clients.end())
    {
        if (clientIterator->second.getNickname() == nickname)
            break ;
        ++clientIterator;
    }
    if (clientIterator == clients.end())
        return ;

    int targetFd = clientIterator->second.getFd();
    if (!channelIt->second.hasClient(targetFd))
        return ;
    if (mode == "+o")
        channelIt->second.addOperator(targetFd);
    else if (mode == "-o")
        channelIt->second.removeOperator(targetFd);

    // reply
}