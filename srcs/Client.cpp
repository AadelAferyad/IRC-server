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
