#include "Server.hpp"


int	main(void)
{
	server srv;
	try
	{
		srv.initServer();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return (0);
}
