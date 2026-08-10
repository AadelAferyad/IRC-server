#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <iostream>

class Channel
{
    private:
        std::string name;
        std::string topic;
        std::string key;
        int userLimit;
        bool inviteOnly;
        bool topicRestricted;
        bool keyProtected;
        bool hasUserLimit;
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

        void promoteNewOperator();

        void addOperator(int fd);
        void removeOperator(int fd);
        bool isOperator(int fd) const;

        void addInvite(int fd);
        void removeInvite(int fd);
        bool isInvited(int fd) const;
                
        bool isInviteOnly() const;
        void setInviteOnly(bool enabled);

        bool isTopicRestricted() const;
        void setTopicRestricted(bool enabled);

        bool isKeyProtected() const;
        void setKey(const std::string &key);
        void removeKey();
        bool checkKey(const std::string &key) const;

        bool isUserLimit() const;
        int getUserLimit() const;

        void setUserLimit(int limit);
        void removeUserLimit();
};

#endif
