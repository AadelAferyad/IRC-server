#include "Server.hpp"

void server::Pass(Client &client, const Command &command)
{
    if (client.isRegistered())
        return ;
    if (command.params.empty())
    {
        sendNumeric(client, ERR_NEEDMOREPARAMS, client.getNickname() + " PASS", "Not enough parameters");
        return ;
    }
    if (command.params[0] != password)
    {
        sendNumeric(client, ERR_PASSWDMISMATCH, client.getNickname(), "Password incorrect");
        return ;
    }
    client.setPassAccepted(true);
    checkRegistration(client);
}