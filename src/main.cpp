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

int main(const int IN argc, const char** IN argv)
{
    if (argc != 5 ||
        std::string(argv[1]).empty() ||
        isInt(argv[2]) == false ||
        std::atoi(argv[2]) >> 16 != 0 ||
        std::string(argv[3]).empty() ||
        std::string(argv[4]).empty() ||
        std::string(argv[4]).find(",") != std::string::npos ||
        std::string(argv[4]).find(" ") != std::string::npos)
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