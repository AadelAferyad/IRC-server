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
    {
        sendNumeric(client, ERR_NOSUCHCHANNEL,
            client.getNickname() + " " + channelName,
            "No such channel");
        return ;
    }
    if (!it->second.hasClient(client.getFd()))
    {
        sendNumeric(client, ERR_NOTONCHANNEL,
            client.getNickname() + " " + channelName,
            "You're not on that channel");
        return ;
    }

    if (command.params.size() == 1)
    {
        // send current topic back to client
        return ;
    }
    if (it->second.isTopicRestricted() &&
        !it->second.isOperator(client.getFd()))
    {
        sendNumeric(client, ERR_CHANOPRIVSNEEDED,
            client.getNickname() + " " + channelName,
            "You're not channel operator");
        return ;
    }
    std::string topic = command.params[1];
    it->second.setTopic(topic);
    // broadcast changed topic to all channel's members
}
