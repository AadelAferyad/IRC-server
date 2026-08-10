#include "Server.hpp"

void server::Kick(Client &client, const Command &command)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, ERR_NOTREGISTERED, client.getNickname(), "You have not registered");
        return ;
    }

    if (command.params.size() < 2)
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS,
            client.getNickname() + " KICK",
            "Not enough parameters");
        return ;
    }

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

    std::map<int, Client>::iterator targetIt = clients.begin();

    while (targetIt != clients.end())
    {
        if (targetIt->second.getNickname() == nickname)
            break ;

        ++targetIt;
    }

    if (targetIt == clients.end())
    {
        sendNumeric(client, ERR_NOSUCHNICK,
            client.getNickname() + " " + nickname,
            "No such nick");
        return ;
    }

    int targetFd = targetIt->second.getFd();

    if (!channelIt->second.hasClient(targetFd))
    {
        sendNumeric(client, ERR_NOTONCHANNEL,
            client.getNickname() + " " + nickname,
            "Target is not on that channel");
        return ;
    }

    std::string reply = ":" + client.getNickname() + "!" +
        client.getUsername() + "@localhost KICK " +
        channelName + " " + nickname + "\r\n";

    const std::set<int>& members = channelIt->second.getClients();
    std::set<int>::const_iterator member = members.begin();

    while (member != members.end())
    {
        queueMsg(*member, reply);
        ++member;
    }

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