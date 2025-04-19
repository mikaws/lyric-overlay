#include "random.hpp"

std::string random::generateRandomString(size_t length)
{
    const std::string charset = "0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;     // seed
    std::mt19937 engine(rd()); // mersenne twister engine
    std::uniform_int_distribution<> dist(0, charset.size() - 1);
    std::string result;
    for (size_t i = 0; i < length; ++i)
    {
        result += charset[dist(engine)];
    }
    return result;
}