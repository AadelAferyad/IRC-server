#include "Server.hpp"

void server::Invite(Client &client, const Command &command)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, ERR_NOTREGISTERED, client.getNickname(), "You have not registered");
        return ;
    }
    if (command.params.size() < 2)
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS, client.getNickname() + " INVITE", "Not enough parameters");
        return ;
    }

    std::string nickname = command.params[0];
    std::string channelName = command.params[1];
    std::map<std::string, Channel>::iterator channelIt = channels.find(channelName);
    if (channelIt == channels.end())
    {
        sendNumeric(client, ERR_NOSUCHCHANNEL, client.getNickname() + " " + channelName, "No such channel");
        return ;
    }
    if (!channelIt->second.hasClient(client.getFd()))
    {
        sendNumeric(client, ERR_NOTONCHANNEL, client.getNickname() + " " + channelName, "You're not on that channel");
        return ;
    }
    if (!channelIt->second.isOperator(client.getFd()))
    {
        sendNumeric(client, ERR_CHANOPRIVSNEEDED, client.getNickname() + " " + channelName, "You're not channel operator");
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
        sendNumeric(client, ERR_NOSUCHNICK, client.getNickname() + " " + nickname, "No such nick");
        return ;
    }

    int targetFd = targetIt->second.getFd();
    if (channelIt->second.hasClient(targetFd))
        return ;
    channelIt->second.addInvite(targetFd);
    sendNumeric(client, RPL_INVITING, client.getNickname() + " " + nickname + " " + channelName, "");
    std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + nickname + " " + channelName + "\r\n";
    queueMsg(targetFd, reply);
}