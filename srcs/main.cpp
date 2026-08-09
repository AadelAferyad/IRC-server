#include "Server.hpp"


int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "USAGE: ./IRC <port> <password>"<< std::endl;
		return (1);
	}
	try
	{
		server srv(av[1], av[2]);
		srv.initServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
