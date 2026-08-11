#include "Server.hpp"


int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "USAGE: ./IRC <port> <password>"<< std::endl;
		return (1);
	}
	server srv(av[1], av[2]);
	if (srv.getFlag())
		return (1);
	try
	{
		signal(SIGINT, server::sigHandler);
		signal(SIGQUIT, server::sigHandler);
		srv.initServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		srv.closeFds();
	}
	return (0);
}
