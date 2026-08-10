#include "Server.hpp"

void server::Part(Client &client, const Command &command)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, ERR_NOTREGISTERED, client.getNickname(), "You have not registered");
        return ;
    }
    if (command.params.empty())
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS,
            client.getNickname() + " PART",
            "Not enough parameters");
        return ;
    }

    std::string channelName = command.params[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end())
    {
        sendNumeric(client, ERR_NOSUCHCHANNEL, client.getNickname() + " " + channelName, "No such channel");
        return ;
    }
    if (!it->second.hasClient(client.getFd()))
    {
        sendNumeric(client, ERR_NOTONCHANNEL, client.getNickname() + " " + channelName, "You're not on that channel");
        return ;
    }

    std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName + "\r\n";
    const std::set<int>& members = it->second.getClients();
    std::set<int>::const_iterator member = members.begin();
    while (member != members.end())
    {
        queueMsg(*member, reply);
        ++member;
    }
    bool wasOperator = it->second.isOperator(client.getFd());
    it->second.removeClient(client.getFd());
    if (it->second.getClients().empty())
    {
        channels.erase(it);
        return ;
    }
    if (wasOperator)
        it->second.promoteNewOperator();
}