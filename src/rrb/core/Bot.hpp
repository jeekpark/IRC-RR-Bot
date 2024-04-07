#pragma once

#include "BSD-GDF/Event/KernelEvent.hpp"
#include "BSD-GDF/Event/KernelQueue.hpp"
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
    static Bot* GetInstance();
    static bool CreateInstance(const std::string& IN serverIP,
                               const int16 IN serverPort,
                               const std::string& IN serverPassword,
                               const std::string& IN channelName);
    static void DeleteInstace();
    bool Connect();
    void Disconnect();
    void Authenticate();
    bool Register();
    void Join();
    void Run();

    void Say(const std::string& IN buf);
private:
    Bot(); // = delete
    Bot(const std::string& IN serverIP,
        const int16 IN serverPort,
        const std::string& IN serverPassword,
        const std::string& IN channelName);
    Bot(const Bot& copy); // = delete;
    Bot& operator=(const Bot& copy);
    virtual ~Bot();
    
    void handleMessage(const std::string& IN message);
    static void signalHandler(int signum);
    std::vector<std::string> split(std::string IN input, const std::string& IN delimiter) const;

    static Bot* sStaticInstance;
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