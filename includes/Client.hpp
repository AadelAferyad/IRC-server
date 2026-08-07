#ifndef CLIENT_HPP
#define CLIENT_HPP


class Client
{
    private:
        int             fd;
        sockaddr_in     address;

        bool            passAccepted;
        bool            nickReceived;
        bool            userReceived;
        bool            registered;

        std::string     nickname;
        std::string     username;
        std::string     realname;

        std::string     buffer;
    public:
        Client(int fd, const sockaddr_in& address);
        ~Client();

        int                 getFd() const;
        sockaddr_in         getAddress() const;
        //getters & setters
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


#endif