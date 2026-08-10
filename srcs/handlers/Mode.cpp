#include "Server.hpp"

void server::Mode(Client &client, const Command &command)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, ERR_NOTREGISTERED, client.getNickname(), "You have not registered");
        return ;
    }

    if (command.params.size() < 2)
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS,
            client.getNickname() + " MODE",
            "Not enough parameters");
        return ;
    }

    std::string channelName = command.params[0];
    std::string mode = command.params[1];

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

    if (mode != "+i" && mode != "-i" &&
        mode != "+t" && mode != "-t" &&
        mode != "+k" && mode != "-k" &&
        mode != "+l" && mode != "-l" &&
        mode != "+o" && mode != "-o")
    {
        sendNumeric(client, ERR_UNKNOWNMODE,
            client.getNickname() + " " + mode,
            "Unknown MODE flag");
        return ;
    }

    if (mode == "+i")
    {
        channelIt->second.setInviteOnly(true);
    }
    else if (mode == "-i")
    {
        channelIt->second.setInviteOnly(false);
    }
    else if (mode == "+t")
    {
        channelIt->second.setTopicRestricted(true);
    }
    else if (mode == "-t")
    {
        channelIt->second.setTopicRestricted(false);
    }
    else if (mode == "+k")
    {
        if (command.params.size() < 3)
        {
            sendNumeric(client, ERR_NEEDMOREPARAMS,
                client.getNickname() + " MODE",
                "Not enough parameters");
            return ;
        }

        if (command.params[2].empty())
            return ;

        channelIt->second.setKey(command.params[2]);
    }
    else if (mode == "-k")
    {
        channelIt->second.removeKey();
    }
    else if (mode == "+l")
    {
        if (command.params.size() < 3)
        {
            sendNumeric(client, ERR_NEEDMOREPARAMS,
                client.getNickname() + " MODE",
                "Not enough parameters");
            return ;
        }

        int limit = std::atoi(command.params[2].c_str());

        if (limit <= 0)
            return ;

        channelIt->second.setUserLimit(limit);
    }
    else if (mode == "-l")
    {
        channelIt->second.removeUserLimit();
    }
    else if (mode == "+o" || mode == "-o")
    {
        if (command.params.size() < 3)
        {
            sendNumeric(client, ERR_NEEDMOREPARAMS,
                client.getNickname() + " MODE",
                "Not enough parameters");
            return ;
        }

        std::string nickname = command.params[2];

        std::map<int, Client>::iterator clientIterator = clients.begin();

        while (clientIterator != clients.end())
        {
            if (clientIterator->second.getNickname() == nickname)
                break ;

            ++clientIterator;
        }

        if (clientIterator == clients.end())
        {
            sendNumeric(client, ERR_NOSUCHNICK,
                client.getNickname() + " " + nickname,
                "No such nick");
            return ;
        }

        int targetFd = clientIterator->second.getFd();

        if (!channelIt->second.hasClient(targetFd))
        {
            sendNumeric(client, ERR_NOTONCHANNEL,
                client.getNickname() + " " + nickname,
                "Target is not on that channel");
            return ;
        }

        if (mode == "+o")
            channelIt->second.addOperator(targetFd);
        else
            channelIt->second.removeOperator(targetFd);
    }

    std::string reply = ":" + client.getNickname() + "!" +
        client.getUsername() + "@localhost MODE " +
        channelName + " " + mode;

    if (command.params.size() >= 3)
        reply += " " + command.params[2];

    reply += "\r\n";

    const std::set<int>& members = channelIt->second.getClients();
    std::set<int>::const_iterator member = members.begin();

    while (member != members.end())
    {
        queueMsg(*member, reply);
        ++member;
    }
}