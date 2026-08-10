#include "Server.hpp"

void server::Privmsg(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return ;
    if (command.params.size() < 2)
        return ;

    std::string target = command.params[0];
    std::string message = command.params[1];

    if (target[0] == '#')
    {
        std::map<std::string, Channel>::iterator it = channels.find(target);
        if (it == channels.end())
        {
            sendNumeric(client, ERR_NOSUCHCHANNEL,
                client.getNickname() + " " + target,
                "No such channel");
            return ;
        }
        if (!it->second.hasClient(client.getFd()))
        {
            sendNumeric(client, ERR_CANNOTSENDTOCHAN,
                client.getNickname() + " " + target,
                "Cannot send to channel");
            return ;
        }

        const std::set<int>& members = it->second.getClients();
        std::set<int>::const_iterator member = members.begin();
        while (member != members.end())
        {
            if (*member != client.getFd())
                //send message to every *member
            member++;
        }
    }
    else
    {
        std::map<int, Client>::iterator it = clients.begin();

        while (it != clients.end())
        {
            if (it->second.getNickname() == target)
            {
                //send message to specific fd;
                break;
            }
            
            it++;
        }
        if (it == clients.end())
        {
            sendNumeric(client, ERR_NOSUCHNICK,
                client.getNickname() + " " + target,
                "No such nick");
            return ;
        }
    }
}
