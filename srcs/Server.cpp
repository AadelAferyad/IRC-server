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
void    Server::Initpoll()
{
    pollfd ListeningSocket;

    ListeningSocket.fd = listenFD;
    ListeningSocket.events = POLLIN;
    ListeningSocket.revents = 0;

    pollFds.push_back(ListeningSocket);
}

void    Server::acceptConnection()
{
    sockaddr_in clientAddress;
    socklen_t clientLength = sizeof(clientAddress);
    
    int ClientFD = accept(listenFD, reinterpret_cast<sockaddr*>(&clientAddress), &clientLength);
    if (ClientFD == -1)
    {
        std::cerr << "ERROR: accept function failed !\n";
        return ;
    }

    if (fcntl(ClientFD, F_SETFL, O_NONBLOCK) == -1)
    {
        close(ClientFD);
        return;
    }

    Client *client = new Client(ClientFD);
    clients[ClientFD] = client;
    pollfd ClientSocket;
    ClientSocket.fd = ClientFD;
    ClientSocket.events = POLLIN;
    ClientSocket.revents = 0;
    pollFds.push_back(ClientSocket);
}

void    Server::run()
{
    SetupListeningSocket();
    Initpoll();

    while (1)
    {
        int ready = poll(&pollFds[0], pollFds.size(), -1);
        if (ready == -1)
            throw std::runtime_error("ERROR: poll function failed !");
            
        for (size_t i = 0; i < pollFds.size(); i++)
        {
            if (pollFds[i].revents & POLLIN) // to know
            {
                if (pollFds[i].fd == listenFD) // to know
                    acceptConnection();
                else
                {
                    // to do
                }
            }
        }
    }
}