#pragma once

#include "common.hpp"

namespace rrb
{

class Core
{
public:
    Core(const std::string& IN serverIP,
         const int16 IN serverPort,
         const std::string& IN serverPassword);
    ~Core();

    bool Init();
    void Run();
private:
    Core(); // = delete
    Core(const Core& core); // = delete
    Core& operator=(const Core& core); // = delete

    const std::string mServerIP;
    const int16 mServerPort;
    const std::string mServerPassword;
    bool mbRunning;
    gdf::KernelQueue mKernelQueue;
    gdf::Network mNetwork;
};

}