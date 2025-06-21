#pragma once
#include <string>

class Client
{
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        std::string _inputBuffer;
    public:
        Client(int fd);
        ~Client();
        //getters
        int getFd() const;
        const std::string& getNicename() const;
        const std::string& getUsername() const;
        std::string& getBuffer();
        //setters
        void setNickname(const std::string&);
        void setUsername(const std::string&);
        void appendToBuffer(const std::string&);
};
