#pragma once

#include "iostream"

class	Client
{
	private:
		int	fd;
		std::string cmdBuffer;
		bool            passAccepted;
		bool            nickReceived;
		bool            userReceived;
		bool            registered;

		std::string     nickname;
		std::string     username;
		std::string     realname;

        std::string     buffer;
	public:
		Client();
		Client(int fd);
		~Client();
		int	getClientFd() const;
		void	addBuffer(char *str);
		std::string	&getBuffer();
		void	closeFds();
		int	getFd() const;
		void	seter(int f);
		bool                isPassAccepted() const;
		bool                isNickReceived() const;
		bool                isUserReceived() const;
		bool                isRegistered() const;
		std::string         getNickname() const;
		std::string         getUsername() const;
		std::string         getRealname() const;
		std::string         getBuffer() const;
		void setPassAccepted(bool value);
		void setNickReceived(bool value);
		void setUserReceived(bool value);
		void setRegistered(bool value);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setRealname(const std::string& realname);
		void setBuffer(const std::string& buffer);
};
