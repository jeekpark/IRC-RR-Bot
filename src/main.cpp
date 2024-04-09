#include "rrb/core/Bot.hpp"

bool isInt(const std::string& literal)
{
	char* endPtr = NULL;
	const char* cStr = literal.c_str();
	long longValue = std::strtol(cStr, &endPtr, 10);
	if (literal.length() != 0
		&& *endPtr == '\0'
		&& (static_cast<long>(std::numeric_limits<int>::min()) <= longValue 
			&& longValue <= static_cast<long>(std::numeric_limits<int>::max())))
		return true;
	else
		return false;
}

bool validateMainArguments(const int32 IN argc, const char** IN argv)
{
    if (argc != 5)
    {
        return false;
    }
    std::string IP = argv[1],
                port = argv[2],
                serverPassword = argv[3],
                channelName = argv[4];

    if (IP.empty())
    {
        return false;
    }

    if (port.empty() ||
        isInt(port) == false ||
        std::atoi(port.c_str()) >> 16 != 0)
    {
        return false;
    }

    if (serverPassword.empty())
    {
        return false;
    }

    if (channelName.empty() ||
        channelName.find(",") != std::string::npos ||
        channelName.find(" ") != std::string::npos ||
        channelName.find("*") != std::string::npos)
    {
        return false;
    }

    return true;
}

int main(const int32 IN argc, const char** IN argv)
{
    if (validateMainArguments(argc, argv) == false)
    {
        return 1;
    }

    if (rrb::Bot::CreateInstance(argv[1],
                                 std::atoi(argv[2]),
                                 argv[3],
                                 argv[4]) == false)
    {
        return 1;
    }
    
    rrb::Bot::GetInstance()->Connect();
    rrb::Bot::GetInstance()->Authenticate();
    rrb::Bot::GetInstance()->Register();
    rrb::Bot::GetInstance()->Join();
    rrb::Bot::GetInstance()->Run();
    rrb::Bot::DeleteInstace();
    return 0;
}