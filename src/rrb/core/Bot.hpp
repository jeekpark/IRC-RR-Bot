#pragma once

#include "common.hpp"
#include <cstring>
#include <netdb.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
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
        if (mKernelQueue.Init() == FAILURE)
        {
            close(mSocket);
            return false;
        }
        if (mKernelQueue.AddReadEvent(mSocket) == FAILURE)
        {
            close(mSocket);
            return false;
        }
        mbConnected = true;
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
        Say(message);
    }

    bool Register()
    {
        gdf::KernelEvent event;
        char c_buf[1024];
        std::memset(c_buf, 0, sizeof(c_buf));
        std::string buf;
        struct timeval start, now;
        gettimeofday(&start, NULL);
        while (true)
        {
            gettimeofday(&now, NULL);
            if (now.tv_sec - start.tv_sec > 5)
                return false;
            while (mKernelQueue.Poll(event))
            {
                if (event.IdentifySocket(mSocket) && event.IsReadType())
                {
                    std::memset(c_buf, 0, sizeof(c_buf));
                    recv(mSocket, c_buf, 1024, 0);
                    buf += c_buf;
                    if (buf.find("PING") != std::string::npos
                        && buf.find(mServerIP) != std::string::npos
                        && buf.find("\r\n") != std::string::npos)
                    {
                        Say("PONG " + mServerIP + " :" + mServerIP + "\r\n");
                        return true;
                    }
                }
            }
        }
    }

    void Join()
    {
        std::string message;
        message += "JOIN #" + mChannelName + "\r\n";
        Say(message);
    }
    void Say(const std::string& IN buf)
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
    int32 GetSocket() const
    {
        return mSocket;
    }
private:

    const std::string mServerIP;
    const int16 mServerPort;
    const std::string mServerPassword;
    const std::string mChannelName;
    bool mbConnected;
    
    int32 mSocket;
    struct sockaddr_in mServerAddress;

    gdf::KernelQueue mKernelQueue;


};

}