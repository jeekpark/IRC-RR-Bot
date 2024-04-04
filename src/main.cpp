
#include "BSD-GDF/Event/KernelEvent.hpp"
#include "BSD-GDF/Event/KernelQueue.hpp"
#include "common.hpp"
#include "rrb/core/Bot.hpp"
#include <unistd.h>

int main()
{
    rrb::Bot a("127.0.0.1",
               6667,
               "hello",
               "random");
    if (a.Connect() == false)
    {
        std::cout << "false" << std::endl;
    }
    else
    {
        std::cout << "true" << std::endl;
    }
    a.Authenticate();
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
            if (str[0] == 'P')
                send(a.GetSocket(), "PONG 10.32.2.7 10.32.2.7", 25, 0);
        }
    }
    pause();
    return 0;
}