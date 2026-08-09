#include "Channel.hpp"

Channel::Channel(const std::string& name) : name(name) {}
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
}

bool Channel::hasClient(int fd) const
{
    return clients.find(fd) != clients.end();
}

const std::set<int>& Channel::getClients() const
{
    return clients;
}
