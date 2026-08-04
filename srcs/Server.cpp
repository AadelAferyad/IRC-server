#include "../includes/Server.hpp"

Server::Server(int _port, const std::string &_password): port(_port), password(_password), listenFD(-1)
{
    if (port <= 0 || port > 65535)
        throw std::runtime_error("ERROR: Invalid port !");
    if (password.empty())
        throw std::runtime_error("ERROR: Password cant be empty !");
}
Server::~Server() {}

void    Server::SetupListeningSocket()
{
    listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFD == -1)
        throw std::runtime_error("ERROR: Creating a TCP Socket failed !");
    
    int enable = 1;
    if (setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
        throw std::runtime_error("ERROR: Configuring Address reuse failed !");

    if (fcntl(listenFD, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("ERROR: Setting NON-BLOCKING failed !");

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenFD, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
        throw std::runtime_error("ERROR: Binding socket failed !");

    if (listen(listenFD, SOMAXCONN) == -1)
        throw std::runtime_error("ERROR: Creating Listening socket failed !");
}

void    Server::run()
{
    SetupListeningSocket();
}