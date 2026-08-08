#pragma once

#include "iostream"

class	Client
{
	private:
		int	fd;
		std::string cmdBuffer;
	public:
		Client();
		~Client();
		int	getClientFd() const;
		void	addBuffer(char *str);
		std::string	getBuffer();
		void	seter(int f);
};
