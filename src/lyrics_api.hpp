#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "curl/curl.h"
#include "include/json.hpp"

using json = nlohmann::json;

class LyricsAPI
{
private:
    CURL *curl;
    std::string last_error;
public:
    struct Lyrics
    {
        unsigned int startTimeMs;
        std::string words;
    };
    std::vector<Lyrics> array;
    bool requestLyrics(std::string music_id);
    const std::string& getLastError() const;
    LyricsAPI();
    ~LyricsAPI();
};