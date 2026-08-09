#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <iostream>

class Channel
{
    private:
        std::string name;
        std::string topic;
        std::set<int> clients;
    public:
        Channel(const std::string &name);
        ~Channel();

        const std::string &getName() const;
        const std::string &getTopic() const;
        void setTopic(const std::string& topic);

        void addClient(int fd);
        void removeClient(int fd);
        bool hasClient(int fd) const;

        const std::set<int> &getClients() const;
};

#endif
