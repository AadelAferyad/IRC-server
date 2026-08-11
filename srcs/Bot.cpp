#include "Bot.hpp"
#include "Server.hpp"
#include "Client.hpp"

Bot::Bot() : nickname("ircBot") {}

Bot::~Bot() {}

std::string Bot::getHelp() const
{
    return "Available commands: !help !commands !usage <command> !about";
}

std::string Bot::getCommands() const
{
    return "IRC commands: PASS NICK USER JOIN PART PRIVMSG KICK INVITE TOPIC MODE QUIT";
}

std::string Bot::getUsage(const std::string &command) const
{
    if (command == "PASS")
        return "PASS <password>";
    if (command == "NICK")
        return "NICK <nickname>";
    if (command == "USER")
        return "USER <username> <hostname> <servername> :<realname>";
    if (command == "JOIN")
        return "JOIN <#channel> [<key>]";
    if (command == "PART")
        return "PART <#channel> [:<reason>]";
    if (command == "PRIVMSG")
        return "PRIVMSG <target> :<message>";
    if (command == "KICK")
        return "KICK <#channel> <nickname> [:<reason>]";
    if (command == "INVITE")
        return "INVITE <nickname> <#channel>";
    if (command == "TOPIC")
        return "TOPIC <#channel> [:<topic>]";
    if (command == "MODE")
        return "MODE <target> <mode> [parameters]";
    if (command == "QUIT")
        return "QUIT [:<reason>]";

    return "Unknown command. Use !commands to see the supported IRC commands.";
}

std::string Bot::getAbout() const
{
    return "ircBot - IRC command and usage help bot.";
}

bool Bot::handleCommand(server &srv, Client &client, const std::string &target, const std::string &message)
{
	std::string response;

	if (message == "!help")
		response = getHelp();
	else if (message == "!commands")
		response = getCommands();
	else if (message == "!about")
		response = getAbout();
	else if (message.compare(0, 7, "!usage ") == 0)
		response = getUsage(message.substr(7));
	else
		return (false);

	std::string reply = ":" + nickname + "!bot@localhost PRIVMSG " + target + " :" + response + "\r\n";
	if (!target.empty() && target[0] == '#')
	{
		srv.sendToChannel(target, reply);
		return (true);
	}
	srv.queueMsg(client.getFd(), reply);
	return (true);
}