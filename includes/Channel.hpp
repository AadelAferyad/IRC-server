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
        std::set<int> operators;
        std::set<int> invited;
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

        void addOperator(int fd);
        void removeOperator(int fd);
        bool isOperator(int fd) const;

        void addInvite(int fd);
        void removeInvite(int fd);
        bool isInvited(int fd) const;
        
        void promoteNewOperator();
};

#endif
