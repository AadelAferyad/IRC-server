#include "Server.hpp"

void server::Privmsg(Client &client, const Command &command)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, ERR_NOTREGISTERED, client.getNickname(), "You have not registered");
        return ;
    }
    if (command.params.size() < 2)
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS,
            client.getNickname() + " PRIVMSG",
            "Not enough parameters");
        return ;
    }

    std::string target = command.params[0];
    std::string message = command.params[1];
    std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
    if (!target.empty() && target[0] == '#')
    {
        std::map<std::string, Channel>::iterator it = channels.find(target);
        if (it == channels.end())
        {
            sendNumeric(client, ERR_NOSUCHCHANNEL, client.getNickname() + " " + target, "No such channel");
            return ;
        }
        if (!it->second.hasClient(client.getFd()))
        {
            sendNumeric(client, ERR_CANNOTSENDTOCHAN, client.getNickname() + " " + target, "Cannot send to channel");
            return ;
        }
        const std::set<int> &members = it->second.getClients();
        for (std::set<int>::const_iterator member = members.begin(); member != members.end(); ++member)
        {
            if (*member == client.getFd())
                continue ;
            if (clients.find(*member) == clients.end())
                continue ;
            queueMsg(*member, reply);
        }
        return ;
    }
    std::map<int, Client>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->second.getNickname() == target)
            break ;
        ++it;
    }
    if (it == clients.end())
    {
        sendNumeric(client, ERR_NOSUCHNICK, client.getNickname() + " " + target, "No such nick");
        return ;
    }
    queueMsg(it->second.getFd(), reply);
}