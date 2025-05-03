#include "lyrics_api.hpp"

LyricsAPI::LyricsAPI()
{
    LyricsAPI::curl = curl_easy_init();
}

LyricsAPI::~LyricsAPI()
{
    if (LyricsAPI::curl)
        curl_easy_cleanup(LyricsAPI::curl);
}

bool LyricsAPI::requestLyrics(std::string music_id)
{
    std::cout << "requesting lyrics api" << std::endl;
    if (!LyricsAPI::curl)
    {
        last_error = "CURL not initialized";
        return false;
    }
    std::string lyricsApiEndpoint = std::getenv("LYRICS_API_ENDPOINT");
    std::string response; // Buffer to store response
    std::string url = lyricsApiEndpoint + "/?trackid=" + music_id;
    std::cout << "Requesting URL: " << url << std::endl;

    curl_easy_setopt(LyricsAPI::curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(LyricsAPI::curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(LyricsAPI::curl, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(LyricsAPI::curl);
    
    if (res != CURLE_OK) {
        last_error = "Failed to make request: " + std::string(curl_easy_strerror(res));
        return false;
    }

    std::cout << "Received response: " << response << std::endl;
    
    try {
        json j = json::parse(response);
        array.clear();
        if (j.contains("lines") && j["lines"].is_array()) {
            for (const auto& line : j["lines"]) {
                Lyrics lyric;
                lyric.startTimeMs = std::stoul(line["startTimeMs"].get<std::string>());
                lyric.words = line["words"].get<std::string>();
                array.push_back(lyric);
            }
            std::cout << "Parsed " << array.size() << " lines of lyrics" << std::endl;
        } else {
            last_error = "Response doesn't contain lyrics lines";
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        last_error = "Failed to parse response: " + std::string(e.what());
        std::cout << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

const std::string& LyricsAPI::getLastError() const {
    return last_error;
}