#pragma once

#include "common.hpp"
#include <cstring>
#include <netdb.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define HOSTNAME "irc.gdf.org"

namespace rrb
{

class Bot
{
public:
    Bot(const std::string& IN serverIP,
        const int16 IN serverPort,
        const std::string& IN serverPassword,
        const std::string& IN channelName)
    : mServerIP(serverIP)
    , mServerPort(serverPort)
    , mServerPassword(serverPassword)
    , mChannelName(channelName)
    , mbConnected(false)
    {
    }
    virtual ~Bot()
    {
        Disconnect();
    }

    bool Connect()
    {
        mSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (mSocket < 0)
            return false;
        std::memset(&mServerAddress, 0, sizeof(mServerAddress));
        mServerAddress.sin_family = AF_INET;
        mServerAddress.sin_port = htons(mServerPort);
        inet_pton(AF_INET, mServerIP.c_str(), &mServerAddress.sin_addr);
        
        if (connect(mSocket,(struct sockaddr *)&mServerAddress, sizeof(mServerAddress)) < 0)
        {
            close(mSocket);
            return false;
        }
        if (fcntl(mSocket, F_SETFL, O_NONBLOCK) < 0)
        {
            close(mSocket);
            return false;
        }
        return true;
    };
    void Disconnect()
    {
        close(mSocket);
    }
    void Authenticate()
    {
        std::string message;
        message += "PASS " + mServerPassword + "\r\n"
                + "NICK roulette\r\n"
                + "USER RussianRoulette RussianRoulette irc.gdf.org :RussianRoulette\r\n";
        sendBlock(message);
    }
    void Join();
    void Say(const std::string& IN message);
    int32 GetSocket() const
    {
        return mSocket;
    }
private:
    void sendBlock(const std::string& IN buf)
    {
        const char* c_buf = buf.c_str();
        std::size_t index = 0;
        while (true)
        {
            std::size_t messageLen = std::strlen(&c_buf[index]);
            std::size_t sendLen = send(mSocket, &c_buf[index], messageLen, 0);
            if (sendLen == messageLen)
                break;
            else
                index += sendLen;
        }
    }
    const std::string mServerIP;
    const int16 mServerPort;
    const std::string mServerPassword;
    const std::string mChannelName;
    bool mbConnected;
    
    int32 mSocket;
    struct sockaddr_in mServerAddress;


};

}