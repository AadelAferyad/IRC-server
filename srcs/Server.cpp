#include "Server.hpp"

server::server(){};
bool isValidPort(std::string &str)
{
	if (str.empty())
		return (false);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return (false);
	}
	long value = std::atol(str.c_str());
	return (value > 0 && value <= 65535);
}
server::server(std::string port, std::string pass)
{
	if (!isValidPort(port))
		throw std::runtime_error("Error invalid port");
	if (pass.empty())
		throw std::runtime_error("Error invalid password empty");
	this->port = std::atoi(port.c_str());
	this->password = pass;
}
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

void	server::queueMsg(int fd, const std::string &msg)
{

	clients[fd].getOutBuffer() += msg;

	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == fd)
		{
			enablePollOut(fds[i]);

			break ;
		}
	}
}

void	server::sendData(struct pollfd &fd)
{
	if (clients[fd.fd].getOutBuffer().empty())
	{
		fd.events &= ~POLLOUT;
		return ;
	}
	std::string	&buff = clients[fd.fd].getOutBuffer();
	ssize_t bytes = send(fd.fd, buff.c_str(), buff.size(), 0);
    	std::cout << "send() returned " << bytes << std::endl;
	if (bytes < 0)
	{
		return ; // ERROR SHOULD BE HANDLED ..
	}
	buff.erase(0, bytes);
	if (buff.empty())
		fd.events &= ~POLLOUT;
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
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == this->socketFd)
					acceptNewCLient();
				else
					if (readData(fds[i].fd))
						i--;
			}
			if (fds[i].revents & POLLOUT)
			{  
				std::cout << "POLLOUT on fd " << fds[i].fd << std::endl;
				sendData(fds[i]);
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

Command server::parse(std::string &str)
{
	Command cmd;

	if (str.empty())
		return cmd;
	std::istringstream ss(str);
	ss >> cmd.cmd_name;
	std::string	temp;
	while (ss >> temp)
		cmd.params.push_back(temp);
	return (cmd);
}

int	server::readData(int fd)
{
	char	buff[1024];
	Command	cmd;

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
	while (true)
	{
		size_t pos = clients[fd].getBuffer().find("\r\n");
		if (pos == std::string::npos)
			break;
		std::string line = clients[fd].getBuffer().substr(0, pos);
		clients[fd].getBuffer().erase(0, pos + 2);
		Command cmd = parse(line);
		dispatcher.dispatchCmd(*this, clients[fd], cmd);
	}
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

void server::enablePollOut(struct pollfd &fd)
{
	fd.events |= POLLOUT;
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

void server::checkRegistration(Client& client)
{
    if (client.isPassAccepted() && client.isNickReceived() && client.isUserReceived())
    {
        client.setRegistered(true);

        sendNumeric(client, RPL_WELCOME, client.getNickname(), "Welcome to the IRC server");
    }
}

void server::sendNumeric(Client &client, int numeric, const std::string &params, const std::string &message)
{
    std::ostringstream oss;

    oss << ":server " << std::setfill('0') << std::setw(3) << numeric << " " << params;
    if (!message.empty())
        oss << " :" << message;
    oss << "\r\n";
    queueMsg(client.getFd(), oss.str());
}