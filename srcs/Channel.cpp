#include "Channel.hpp"

Channel::Channel(const std::string& name) : name(name), key(""), userLimit(0), inviteOnly(false), topicRestricted(false), keyProtected(false), hasUserLimit(false) {}
Channel::~Channel() {}

const std::string& Channel::getName() const
{
    return name;
}
const std::string& Channel::getTopic() const
{
    return topic;
}
void Channel::setTopic(const std::string& topic)
{
    this->topic = topic;
}

void Channel::addClient(int fd)
{
    clients.insert(fd);
}

void Channel::removeClient(int fd)
{
    clients.erase(fd);
    operators.erase(fd);
}

bool Channel::hasClient(int fd) const
{
    return clients.find(fd) != clients.end();
}

const std::set<int>& Channel::getClients() const
{
    return clients;
}

void Channel::addOperator(int fd)
{
    operators.insert(fd);
}

void Channel::removeOperator(int fd)
{
    operators.erase(fd);
}

bool Channel::isOperator(int fd) const
{
    return operators.find(fd) != operators.end();
}

void Channel::addInvite(int fd)
{
    invited.insert(fd);
}

void Channel::removeInvite(int fd)
{
    invited.erase(fd);
}

bool Channel::isInvited(int fd) const
{
    return invited.find(fd) != invited.end();
}

void Channel::promoteNewOperator()
{
    if (clients.empty())
        return ;
    operators.insert(*clients.begin());
}

bool Channel::isInviteOnly() const
{
    return inviteOnly;
}

void Channel::setInviteOnly(bool value)
{
    inviteOnly = value;
}

bool Channel::isTopicRestricted() const
{
    return topicRestricted;
}

void Channel::setTopicRestricted(bool value)
{
    topicRestricted = value;
}

bool Channel::isKeyProtected() const
{
    return keyProtected;
}

void Channel::setKey(const std::string &newKey)
{
    key = newKey;
    keyProtected = true;
}

void Channel::removeKey()
{
    key.clear();
    keyProtected = false;
}

bool Channel::checkKey(const std::string &providedKey) const
{
    return key == providedKey;
}

bool Channel::isUserLimit() const
{
    return hasUserLimit;
}

int Channel::getUserLimit() const
{
    return userLimit;
}

void Channel::setUserLimit(int limit)
{
    userLimit = limit;
    hasUserLimit = true;
}

void Channel::removeUserLimit()
{
    userLimit = 0;
    hasUserLimit = false;
}