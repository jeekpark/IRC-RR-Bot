#include "Core.hpp"
#include "BSD-GDF/Logger/GlobalLogger.hpp"
#include <unistd.h>

namespace rrb
{

Core::Core(const std::string& IN serverIP,
           const int16 IN serverPort,
           const std::string& IN serverPassword)
: mServerIP(serverIP)
, mServerPort(serverPort)
, mServerPassword(serverPassword)
, mbRunning(false)
{
}

Core::~Core()
{   
}

bool Core::Init()
{
    return true;
}

}