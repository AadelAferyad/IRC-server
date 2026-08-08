#ifndef CHANNEL_HPP
#define CHANNEL_HPP


class Channel
{
    private:
        std::string name;
        std::set<int> clients;
    public:
        Channel(std::string name);
        ~Channel();

        const std::string& getName() const;

        void addClient(int fd);
        void removeClient(int fd);
        bool hasClient(int fd) const;

        const std::set<int>& getClients() const;
};

#endif
