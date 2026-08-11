#include "Server.hpp"

server::server(){};
bool server::sig = true;
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
int	server::getFlag() const
{
	return (flag);
}
server::server(std::string port, std::string pass)
{
	if (!isValidPort(port))
	{
		std::cerr << "Error invalid port" << std::endl;
		flag = 1;
	}
	else if (pass.empty())
	{
		std::cerr << "Error invalid password empty" << std::endl;
		flag = 1;
	}
	else
	{
		this->port = std::atoi(port.c_str());
		this->password = pass;
		flag = 0;
	}
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

void server::queueMsg(int fd, const std::string &msg)
{
    std::map<int, Client>::iterator it = clients.find(fd);

    if (it == clients.end())
        return ;

    it->second.getOutBuffer() += msg;

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
	if (bytes < 0)
	{
		std::cerr << "Error sending" << std::endl;
		return ;
	}
	buff.erase(0, bytes);
	if (buff.empty())
		fd.events &= ~POLLOUT;
}

void server::run()
{
    std::cout << "\033[2J\033[H" << std::flush;
    std::cout << GREEN << "			Server created" << WHITE << std::endl;

    while (server::sig)
    {
        int ready = poll(&fds[0], fds.size(), -1);
        if (ready == -1)
	{
		if (errno == EINTR)
			continue;
            throw std::runtime_error("Failed to execute poll");
	}
        for (size_t i = 0; i < fds.size();)
        {
            if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                int fd = fds[i].fd;

                if (fd != this->socketFd)
                {
                    clearClient(fd);
                    close(fd);
                    continue;
                }
            }
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == this->socketFd)
                {
                    acceptNewCLient();
                }
                else
                {
                    int fd = fds[i].fd;

                    if (readData(fd))
                        continue;
                }
            }
            if (i >= fds.size())
                break;
            if (fds[i].revents & POLLOUT)
                sendData(fds[i]);
            ++i;
        }
    }
    closeFds();
}

void	server::sigHandler(int signum)
{
	(void)signum;
	server::sig = false;
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

    std::string param;

    while (ss >> param)
    {
        if (param[0] == ':')
        {
            std::string trailing = param.substr(1);
            std::string rest;

            std::getline(ss, rest);

            if (!rest.empty())
                trailing += rest;

            cmd.params.push_back(trailing);
            break ;
        }

        cmd.params.push_back(param);
    }

    return cmd;
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

void server::clearClient(int fd)
{
    for (std::map<std::string, Channel>::iterator it = channels.begin();
        it != channels.end();)
    {
        bool wasOperator = it->second.isOperator(fd);

        it->second.removeClient(fd);
        it->second.removeInvite(fd);

        if (it->second.getClients().empty())
        {
            std::map<std::string, Channel>::iterator toErase = it;
            ++it;
            channels.erase(toErase);
        }
        else
        {
            if (wasOperator)
                it->second.promoteNewOperator();
            ++it;
        }
    }

    for (size_t i = 0; i < fds.size(); ++i)
    {
        if (fds[i].fd == fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }

    clients.erase(fd);
}

void server::enablePollOut(struct pollfd &fd)
{
	fd.events |= POLLOUT;
}

void server::closeFds()
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->first != -1)
			close(it->first);
	}
	if (socketFd != -1)
	{
		close(socketFd);
		socketFd = -1;
	}
	clients.clear();
	fds.clear();
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

void server::sendToChannel(const std::string &channelName, const std::string &message)
{
	std::map<std::string, Channel>::iterator channel = channels.find(channelName);
	if (channel == channels.end())
		return ;

	const std::set<int> &members = channel->second.getClients();
	for (std::set<int>::const_iterator member = members.begin(); member != members.end(); ++member)
	{
		if (clients.find(*member) == clients.end())
			continue ;
		queueMsg(*member, message);
	}
}
