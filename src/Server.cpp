#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <arpa/inet.h>

Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverSocket(-1), _fdMax(0)
{
}

void Server::init()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);
    if (bind(_serverSocket, (struct sockaddr_in*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(_serverSocket, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFds);
    FD_SET(_serverSocket, &_readfds);
    _fdMax = _serverSocket + 1;

    std::cout << "Server listening on port " << _port << std::endl;
}

void Server::run()
{
    while (true)
    {
        fd_set readFds = this->_readFds;
        fd_set writeFds = this->_writeFds;

        if (select(_fdMax, &readFds, &writeFds, NULL, NULL) < 0)
        {
            perror("select");
            continue;
        }
        for (int fd=0; fd < _fdMax; ++fd)
        {
            if (FD_ISSET(fd, &readFds))
            {
                if (fd == _serverSocket)
                    acceptNewClient();
                else
                    receiveClientMessage(fd);
            }
        }
    }
}

void Server::acceptNewClient()
{
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientFd = accept(_serverSocket, (struct sockaddr_in*)&clientAddr, &len);
    if (clientFd < 0) return;
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    FD_SET(clientFd, &_readFds);
    if (clientFd >= _fdMax)
        _fdMax = clientFd + 1;
    _clients[clientFd] = Client(clientFd);
    std::cout << "New connection: " << clientFd << std::endl;
}

void Server::disconnetClient(int clientFd, const std::string& reason)
{
    std::cout << "Disconnecting :" << clientFd << ": " << reason << std::endl;
    close(clientFd);
    FD_CLR(clientFd, &_readFds);
    _clients.erase(clientFd);
}

void Server::receiveClientMessage(int clientFd)
{
    char buffer[1024];
    ssize_t bytes_read = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0)
    {
        disconnetClient(clientFd, "Read error or client closed the connection");
        return ;
    }
    buffer[bytes_read] = '\0';
    _clients[clientFd].appendToBuffer(buffer);
    processClientBuffer(clientFd);
}

void Server::processClientBuffer(int clientFd)
{
    std::string& buf = _clients[clientFd].getBuffer();
    size_t position;
    while((pos = buf.find('\r\n')) != std::string::npos)
    {
        std::string line = buf.substr(0, position);
        buf.erase(0, position + 2);
        handleCommand(clientFd, line);
    }
}

void Server::handleCommand(int clientFd, const std::string& line)
{
    std::cout << "Received from " << clientFd << ": " << line << std::endl;
    // TO DO: dispatch to command handlers (Person B's job)
}

Client& Server::getClient(int clientFd) {
    return _clients[clientFd];
}

Channel& Server::getOrCreateChannel(const std::string& name) {
    return _channels[name];
}

bool Server::nicknameExists(const std::string& nick)
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getNickname() == nick)
            return true;
    }
    return false;
}

void Server::cleanup()
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        close(it->first);
    close(_serverSocket);
    _clients.clear();
    _channels.clear();
}
