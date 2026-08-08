#include "Client.hpp"


Client::Client() {};
Client::~Client() {};
int	Client::getClientFd() const
{
	return (fd);
}
void	Client::seter(int f)
{
	fd = f;
}

void	Client::addBuffer(char *str)
{
	this->cmdBuffer += str;
}

std::string	Client::getBuffer()
{return (cmdBuffer);}
