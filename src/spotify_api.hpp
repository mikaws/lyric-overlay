#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include "curl/curl.h"
#include "include/json.hpp"

using json = nlohmann::json;

class SpotifyAPI {
private:
    struct TokenInfo {
        std::string access_token;
        std::string token_type;
        std::string scope;
        std::string refresh_token;
        long expires_in;
        std::chrono::system_clock::time_point created_at;
    };
    CURL* curl;
    TokenInfo token_info;
    const std::string client_id;
    const std::string client_secret;
    const std::string token_file = "spotify_token.json";
    std::string last_error;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    bool noToken();
    bool isTokenExpired();
    void saveTokenToFile();
    bool loadTokenFromFile();
    bool authenticate();
    bool refresh();

public:
    struct TrackInfo {
        std::string id;
        unsigned int timestamp;
        unsigned int progress_ms;
    };
    TrackInfo track_info;
    SpotifyAPI(const std::string& client_id, const std::string& client_secret);
    ~SpotifyAPI();
    std::string getAccessToken();
    bool getCurrentTrackRequest();
    const std::string& getLastError() const;
};