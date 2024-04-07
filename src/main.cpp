
#include "BSD-GDF/Event/KernelEvent.hpp"
#include "BSD-GDF/Event/KernelQueue.hpp"
#include "common.hpp"
#include "rrb/core/Bot.hpp"
#include <unistd.h>

int main()
{
    srand(time(0));
    rrb::Bot a("127.0.0.1",
               6667,
               "hello",
               "random");
    if (a.Connect() == false)
        return 0;
    a.Authenticate();
    if (a.Register() == false)
        return 0;
    a.Join();
    gdf::KernelQueue kernelQueue;
    kernelQueue.Init();
    kernelQueue.AddReadEvent(a.GetSocket());
    while (true)
    {
        gdf::KernelEvent event;
        while (kernelQueue.Poll(event))
        {
            const int buffer_size = 2000;
            char buf[buffer_size];
            memset(buf, 0, buffer_size);
            recv(a.GetSocket(), buf, buffer_size, 0);
            std::string str = buf;
            if (
                str.find("!roullete play") != std::string::npos
            )
            {
                if (rand() % 6)
                {
                    a.Say("PRIVMSG #random :you survived\r\n");
                    //a.Say("KICK #random wasavi :reason\r\n");
                }
                else
                {
                    
                    a.Say("PRIVMSG #random :you died\r\n");
                    a.Say("PRIVMSG #random :          ^         \r\n");
                    a.Say("PRIVMSG #random :         | |        \r\n");
                    a.Say("PRIVMSG #random :       @#####@      \r\n");
                    a.Say("PRIVMSG #random :     (###   ###)-.  \r\n");
                    a.Say("PRIVMSG #random :   .(###     ###) \\ \r\n");
                    a.Say("PRIVMSG #random :  /  (###   ###)   )\r\n");
                    a.Say("PRIVMSG #random : (=-  .@#####@|_--\"    잘 가라.\r\n");
                    a.Say("PRIVMSG #random : /\\    \\_|l|_/ (\\\r\n");
                    a.Say("PRIVMSG #random :(=-\\     |l|    /   \r\n");
                    a.Say("PRIVMSG #random : \\  \\.___|l|___/    \r\n");
                    a.Say("PRIVMSG #random : /\\      |_|   /    \r\n");
                    a.Say("PRIVMSG #random :(=-\\._________/\\    \r\n");
                    a.Say("PRIVMSG #random : \\             /    \r\n");
                    a.Say("PRIVMSG #random :   \\._________/     \r\n");
                    a.Say("PRIVMSG #random :     #  ----  #     \r\n");
                    a.Say("PRIVMSG #random :     #   __   #       \r\n");
                    a.Say("PRIVMSG #random :     \\########/      \r\n");
                    a.Say("PRIVMSG #random :\r\n");
                    a.Say("PRIVMSG #random :         V\r\n");
                    a.Say("PRIVMSG #random :             V\r\n");
                    a.Say("PRIVMSG #random :           V\r\n");
                }
                
            }
            else
            {
                std::cout << buf << std::endl;
            }
            
        }
    }
    pause();
    return 0;
}