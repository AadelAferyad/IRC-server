#include "../includes/Server.hpp"

void Server::Pass(Client &client, const Command &command)
{
    if (client.isRegistered())
        return ;
    if (command.params.empty())
        return ;
    if (command.params[0] != password)
        return ;
    client.setPassAccepted(true);
    
    checkRegistration(client);
}
