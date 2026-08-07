#include "../includes/Client.hpp"

Client::Client(int fd, const sockaddr_in& address): fd(fd), address(address), passAccepted(false),
      nickReceived(false),
      userReceived(false),
      registered(false)
{}

Client::~Client() {}


int Client::getFd() const
{
    return fd;
}

sockaddr_in Client::getAddress() const
{
    return address;
}

bool Client::isPassAccepted() const
{
    return passAccepted;
}

bool Client::isNickReceived() const
{
    return nickReceived;
}

bool Client::isUserReceived() const
{
    return userReceived;
}

bool Client::isRegistered() const
{
    return registered;
}

std::string Client::getNickname() const
{
    return nickname;
}

std::string Client::getUsername() const
{
    return username;
}

std::string Client::getRealname() const
{
    return realname;
}

std::string Client::getBuffer() const
{
    return buffer;
}

void Client::setPassAccepted(bool value)
{
    passAccepted = value;
}

void Client::setNickReceived(bool value)
{
    nickReceived = value;
}

void Client::setUserReceived(bool value)
{
    userReceived = value;
}

void Client::setRegistered(bool value)
{
    registered = value;
}

void Client::setNickname(const std::string& nickname)
{
    this->nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
    this->username = username;
}

void Client::setRealname(const std::string& realname)
{
    this->realname = realname;
}

void Client::setBuffer(const std::string& buffer)
{
    this->buffer = buffer;
}