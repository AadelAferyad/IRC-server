#pragma once

#define RED "\e[1;31m"
#define WHITE "\e[0;37m"
#define GREEN "\e[1;32m"
#define CYAN "\e[1;36m" 
#include <iostream>
#include <vector> 
#include <map> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include "Client.hpp"
#include <string.h>
class server
{
	private:
		int	socketFd;
		int	port;
		std::vector<struct pollfd> fds;
		std::map<int, Client> clients;
	public:
		server();
		server(const server &obj);
		server &operator=(const server &obj);
		~server();
		void	initServer();
		void	createSocket();
		void	configSocket();
		void	bindSocket();
		void	passiveSocket();
		void	run();
		void	acceptNewCLient();
		int	readData(int fd);
		void	clearClient(int fd);
		void	closeFds();

};
