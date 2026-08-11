#ifndef BOT_HPP
#define BOT_HPP

#include <string>

class server;
class Client;

class Bot
{
    private:
        std::string nickname;

        std::string getHelp() const;
        std::string getCommands() const;
        std::string getUsage(const std::string &command) const;
        std::string getAbout() const;
    public:
        Bot();
        ~Bot();

        bool handleCommand(server &srv, Client &client, const std::string &target, const std::string &message);
};

#endif