#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _nickname(""), _username(""), _inputBuffer("")
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

const std::string& Client::getNicename() const
{
    return _nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

std::string& Client::getBuffer()
{
    return _inputBuffer;
}

void Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
    _username = username;
}

void Client::appendToBuffer(const std::string& data)
{
    _inputBuffer += data;
}