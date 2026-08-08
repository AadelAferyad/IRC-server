#include "Server.hpp"

void server::User(Client &client, const Command &command)
{
    if (client.isRegistered())
        return ;
    if (command.params.size() < 4)
        return ;
    client.setUsername(command.params[0]);
    client.setRealname(command.params[3]);
    client.setUserReceived(true);

    checkRegistration(client);
}
