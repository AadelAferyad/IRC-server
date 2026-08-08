#include "Server.hpp"

server::server(){};
server::server(const server &obj) { (void) obj;};
server &server::operator=(const server &obj)
{
	(void) obj;
	return (*this);
}
server::~server() {};
void	server::initServer()
{
	struct pollfd fd;
	this->port = 4982; //prot from parsing
	createSocket();
	configSocket();
	bindSocket();
	passiveSocket();
	fd.fd = this->socketFd;
	fd.events = POLLIN;
	fd.revents = 0;
	fds.push_back(fd);
	run();
}

void	server::createSocket()
{
	this->socketFd = socket(AF_INET, SOCK_STREAM, 0);	
	if (this->socketFd == -1)
		throw std::runtime_error("Failed to create a socket");
}
void	server::configSocket()
{
	int	flag = 1;
	if (setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
		throw std::runtime_error("Failed to make this socket reused (port and address)");
	if (fcntl(this->socketFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to make this socket non blocking socket");
}

void	server::bindSocket()
{
	struct sockaddr_in	addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(this->socketFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
		throw std::runtime_error("Failed to bind ip address and port to this socket");
}
void	server::passiveSocket()
{
	if (listen(this->socketFd, SOMAXCONN) == -1)
		throw std::runtime_error("Failed to convert socket status from bound to liten");

}

void	server::run()
{
    	std::cout << "\033[2J\033[H" << std::flush;
	std::cout << GREEN << "			Server created"<< WHITE<< std::endl;
	while (true)
	{
		int ready = poll(&fds[0], fds.size(), -1);
		if (ready == -1)
			throw std::runtime_error("Failed to execute poll");
		for (int i = 0; i < static_cast <int> (fds.size()); i++)
		{
			if (ready == 0)
				break ;
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == this->socketFd)
					acceptNewCLient();
				else
					if (readData(fds[i].fd))
						i--;
				ready--;
			}
		}
	}
	closeFds();
}

void	server::acceptNewCLient()
{
	struct pollfd fd;
	struct sockaddr_in	addr;
	Client client;
	socklen_t	len = sizeof(addr);


	int acc = accept(this->socketFd, reinterpret_cast<struct sockaddr*>(&addr), &len);

	if (acc == - 1)	
		throw std::runtime_error("Failed to accept new client");
	if (fcntl(acc, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to to turn new client file descriptor to non-blocking");
	fd.fd = acc;
	fd.events = POLLIN;
	fd.revents = 0;
	client.seter(acc);
	clients[acc] = client;
	fds.push_back(fd);
	
	std::cout << CYAN << "New Client connecter " << WHITE<< std::endl;
}
int	server::readData(int fd)
{
	char	buff[1024];

	memset(buff, 0, sizeof(buff));

	ssize_t	bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	if (bytes <= 0)
	{
		clearClient(fd);
		std::cout << RED << "CLient Disconnected" << WHITE<< std::endl;
		close(fd);
		return (1);
	}
	buff[bytes] = '\0';
	clients[fd].addBuffer(buff);
	parse(clients[fd]);
	//excute dispatch
	return 0;
}

void	server::clearClient(int fd)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == fd)
		{
			fds.erase(fds.begin() + i);
			break ;
		}
	}
	clients.erase(fd);
}

void	server::closeFds()
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		close(clients[i].getClientFd());
	}
	if (this->socketFd != -1)
		close(socketFd);
	std::cout << RED << "Server Disconnected" << WHITE << std::endl;
}

void    Server::checkRegistration(Client& client)
{
    if (client.isPassAccepted() && client.isNickReceived() && client.isUserReceived())
        client.setRegistered(true);
}
