#pragma once

class	Client
{
	private:
		int	fd;
	public:
		Client();
		~Client();
		int	getClientFd() const;
		void	seter(int f);
};
