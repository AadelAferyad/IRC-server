#include "Server.hpp"

void server::Join(Client &client, const Command &command)
{
    if (!client.isRegistered())
        return ;
    if (command.params.empty())
        return ;

    std::string channelName = command.params[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end())
    {
        channels.insert(std::make_pair(channelName, Channel(channelName)));
        it = channels.find(channelName);
        it->second.addClient(client.getFd());
        it->second.addOperator(client.getFd());
        std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN " + channelName + "\r\n";
        queueMsg(client.getFd(), reply);
        std::string names = "@" + client.getNickname();
        sendNumeric(client, RPL_NAMREPLY, client.getNickname() + " = " + channelName, names);
        sendNumeric(client, RPL_ENDOFNAMES, client.getNickname() + " " + channelName, "End of /NAMES list.");
        return ;
    }

    if (it->second.hasClient(client.getFd()))
        return ;

    if (it->second.isKeyProtected())
    {
        if (command.params.size() < 2)
            return ;
        std::string providedKey = command.params[1];
        if (!it->second.checkKey(providedKey))
            return ;
    }

    if (it->second.isInviteOnly())
    {
        if (!it->second.isInvited(client.getFd()))
            return ;
    }

    if (it->second.isUserLimit())
    {
        if (static_cast<int>(it->second.getClients().size()) >= it->second.getUserLimit())
            return ;
    }

    it->second.addClient(client.getFd());

    if (it->second.isInviteOnly())
        it->second.removeInvite(client.getFd());

    std::string reply = ":" + client.getNickname()
                      + "!" + client.getUsername()
                      + "@localhost JOIN "
                      + channelName + "\r\n";

    queueMsg(client.getFd(), reply);

    std::string names;

    const std::set<int>& members = it->second.getClients();

    std::set<int>::const_iterator member = members.begin();

    while (member != members.end())
    {
        std::map<int, Client>::iterator clientIt;
        clientIt = clients.find(*member);

        if (clientIt != clients.end())
        {
            if (it->second.isOperator(*member))
                names += "@";

            names += clientIt->second.getNickname();
            names += " ";
        }

        ++member;
    }

    sendNumeric(
        client,
        RPL_NAMREPLY,
        client.getNickname() + " = " + channelName,
        names
    );

    sendNumeric(
        client,
        RPL_ENDOFNAMES,
        client.getNickname() + " " + channelName,
        "End of /NAMES list."
    );
}