#include "Server.hpp"

void server::Topic(Client &client, const Command &command)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, ERR_NOTREGISTERED, client.getNickname(), "You have not registered");
        return ;
    }
    if (command.params.empty())
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS, client.getNickname() + " TOPIC", "Not enough parameters");
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
    if (command.params.size() == 1)
    {
        std::string topic = it->second.getTopic();
        if (topic.empty())
            sendNumeric(client, RPL_NOTOPIC, client.getNickname() + " " + channelName, "No topic is set");
        else
            sendNumeric(client, RPL_TOPIC, client.getNickname() + " " + channelName, topic);
        return ;
    }
    if (it->second.isTopicRestricted() && !it->second.isOperator(client.getFd()))
    {
        sendNumeric(client, ERR_CHANOPRIVSNEEDED, client.getNickname() + " " + channelName, "You're not channel operator");
        return ;
    }
    std::string topic = command.params[1];
    it->second.setTopic(topic);
    std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost TOPIC " + channelName + " :" + topic + "\r\n";
    const std::set<int>& members = it->second.getClients();
    std::set<int>::const_iterator member = members.begin();
    while (member != members.end())
    {
        queueMsg(*member, reply);
        ++member;
    }
}