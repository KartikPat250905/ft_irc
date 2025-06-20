#pragma once
#include <iostream>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server
{
    private:
        int _serverSocket;
        int _port;
        std::string _password;
        std::map<int, Client> _clients;
        std::map<std::string, Channel> _channels;
        fd_set _readFds;
        fd_set _writeFds;
        int _fdMax;
    public:
        Server(int port, const std::string& password);
        ~Server();
        void init();// Create, bind, and listen on socket
        void run();// Main loop: accept connections, read/write
        void cleanup();
        void acceptNewClient();
        void disconnetClient(int clientFd, const std::string& reason);
        void receiveClientMessage(int clientFd);// Read from client, buffer input
        void processClientBuffer(int clientFd);// Split lines, call command handlers
        void handleCommand(int clientFd, const std::string& line);  // Route one full command
        bool nicknameExists(const std::string& nick);
};

Client& getClient(int clientFd);// Return a reference to a client
Channel& getOrCreateChannel(const std::string& name);// Return or create a channel