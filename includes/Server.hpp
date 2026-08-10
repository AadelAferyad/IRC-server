#pragma once

#define RED "\e[1;31m"
#define WHITE "\e[0;37m"
#define GREEN "\e[1;32m"
#define CYAN "\e[1;36m" 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
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
#include "Dispatcher.hpp"
#include "Channel.hpp"
#include "Numeric.hpp"

typedef struct Command 
{
     std::string cmd_name;
     std::vector<std::string> params;
} Command;

class Dispatcher;
class server
{
	private:
		int								socketFd;
		int								port;
		std::string						password;
		std::vector<struct pollfd> 		fds;
		std::map<int, Client> 			clients;
		std::map<std::string, Channel>	channels;
		// dispatcher part
		Dispatcher                  	dispatcher;
		void                        	checkRegistration(Client& client);
	public:
		server();
		server(std::string port, std::string pass);
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
		int		readData(int fd);
		void	clearClient(int fd);
		void	closeFds();
		Command	parse(std::string &);
		void	enablePollOut(struct pollfd &fd);
		void	sendData(struct pollfd &fd);

		// cmd handlers
		void 	Pass(Client&, const Command&);
		void 	Nick(Client&, const Command&);
		bool 	nicknameExists(const std::string&);
		void 	User(Client&, const Command&);
		void 	Join(Client&, const Command&);
		void 	Part(Client&, const Command&);
		void 	Privmsg(Client&, const Command&);
		void 	Quit(Client&, const Command&);
		void 	Topic(Client&, const Command&);
		void 	Invite(Client&, const Command&);
		void 	Kick(Client&, const Command&);
		void 	Mode(Client&, const Command&);

		void	queueMsg(int fd, const std::string &msg);
		void	sendNumeric(Client &client, int numeric, const std::string &params, const std::string &message);
};
