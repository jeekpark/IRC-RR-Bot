#include "Bot.hpp"
#include <cstdlib>
#include <new>
#include <sys/signal.h>
#include <vector>

namespace rrb
{

Bot* Bot::sStaticInstance = NULL;

Bot* Bot::GetInstance()
{
    return sStaticInstance;
}

bool Bot::CreateInstance(const std::string &serverIP,
                         const int16 serverPort,
                         const std::string &serverPassword,
                         const std::string &channelName)
{
    sStaticInstance = new (std::nothrow) Bot(serverIP,
                                       serverPort,
                                       serverPassword,
                                       channelName);
    if (sStaticInstance == NULL)
        return false;
    else
        return true;
}

void Bot::DeleteInstace()
{
    delete Bot::sStaticInstance;
    Bot::sStaticInstance = NULL;
}

Bot::Bot(const std::string& IN serverIP,
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

Bot::~Bot()
{
    Disconnect();
}

bool Bot::Connect()
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
    signal(SIGINT, Bot::signalHandler);
    return true;
};
void Bot::Disconnect()
{
    close(mSocket);
}
void Bot::Authenticate()
{
    std::string message;
    message += "PASS " + mServerPassword + "\r\n"
            + "NICK roulette\r\n"
            + "USER RussianRoulette RussianRoulette irc.gdf.org :RussianRoulette\r\n";
    Say(message);
}

bool Bot::Register()
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
                if (buf[0] == 'P'
                    && buf.find("PING") != std::string::npos
                    && buf.find(mServerIP) != std::string::npos
                    && buf.find("\r\n") != std::string::npos)
                {
                    Say("PONG " + mServerIP + " :" + mServerIP + "\r\n");
                    return true;
                }
                else if (buf.find("433 roulette :roulette") != std::string::npos)
                {
                    Say("QUIT :leaving\r\n");
                    exit(1);
                }
            }
        }
    }
}

void Bot::Join()
{
    std::string message;
    message += "JOIN #" + mChannelName + "\r\n";
    Say(message);
}

void Bot::Run()
{
    const uint64 bufferSize = 2000;
    char c_buf[bufferSize];
    std::string buf;
    std::memset(c_buf, 0, bufferSize);
    while (mbConnected)
    {
        gdf::KernelEvent event;
        while (mKernelQueue.Poll(event))
        {
            if (event.IdentifySocket(mSocket) &&
                event.IsReadType())
            {
                ssize_t len = recv(mSocket, c_buf, bufferSize - 1, 0);
                c_buf[len] = '\0';
                buf += c_buf;
                for (std::size_t findIndex = buf.find("\r\n");
                     findIndex != std::string::npos;
                     findIndex = buf.find("\r\n"))
                {
                    handleMessage(buf.substr(0, findIndex));
                    buf.erase(0, findIndex + 2);
                }
            }
            
        }
    }
}


void Bot::Say(const std::string& IN buf)
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

void Bot::handleMessage(const std::string& IN message)
{
    if (message.size() > 510)
        return ;
    std::vector<std::string> messageVector = split(message, " ");

        
    if (messageVector.size() == 5 &&
        messageVector[0][0] == ':' &&
        messageVector[1] == "PRIVMSG" &&
        messageVector[2] == "#" + mChannelName &&
        messageVector[3] == ":!roulette" &&
        messageVector[4] == "play")
    {
        if (rand() % 6)
        {
            Say("PRIVMSG #" + mChannelName + " :You survived\r\n");
        }
        else
        {
            Say("PRIVMSG #" + mChannelName + " :You died\r\n");
            Say(std::string("KICK #") + mChannelName + " " + &messageVector[0][1] + " :unluncky\r\n");
        }
    }
    else if (messageVector.size() >= 4 &&
             messageVector[1] == "KICK" &&
             messageVector[2] == "#" + mChannelName &&
             messageVector[3] == "roulette")
    {
        exit(0);
    }
}


std::vector<std::string> Bot::split(std::string IN input, const std::string& IN delimiter) const
{
    std::vector<std::string> result;
    std::size_t pos = 0;
    std::string token = "";
    while ((pos = input.find(delimiter)) != std::string::npos)
    {
        token = input.substr(0, pos);
        if (token != "")
        {
            result.push_back(token);
        }
        input.erase(0, pos + delimiter.size());
    }
    result.push_back(input);
    return result;
}

void Bot::signalHandler(int signum)
{
    if (Bot::GetInstance())
        Bot::GetInstance()->Say("QUIT :leaving\r\n");
    std::exit(signum);
}

}