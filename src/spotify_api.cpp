#include "spotify_api.hpp"
#include "random.hpp"

using json = nlohmann::json;

// constructor
SpotifyAPI::SpotifyAPI(const std::string &client_id, const std::string &client_secret)
    : client_id(client_id), client_secret(client_secret)
{
    SpotifyAPI::curl = curl_easy_init();
    loadTokenFromFile();
}

// deconstructor
SpotifyAPI::~SpotifyAPI()
{
    if (SpotifyAPI::curl)
        curl_easy_cleanup(SpotifyAPI::curl);
}

// @@@ token management @@@

std::string SpotifyAPI::getAccessToken()
{
    if (noToken())
    {
        std::cout << "No token found, starting authentication process..." << std::endl;
        if (!authenticate())
        {
            std::cout << "Authentication failed: " << last_error << std::endl;
            return "";
        }
    }
    else if (isTokenExpired())
    {
        std::cout << "Token expired, refreshing..." << std::endl;
        if (!refresh())
        {
            std::cout << "Token refresh failed: " << last_error << std::endl;
            return "";
        }
    }
    return token_info.access_token;
}

bool SpotifyAPI::noToken() {
    return token_info.access_token.empty();
}

bool SpotifyAPI::isTokenExpired()
{
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - token_info.created_at).count();
    return diff >= token_info.expires_in;
}

void SpotifyAPI::saveTokenToFile()
{
    json j;
    j["access_token"] = token_info.access_token;
    j["token_type"] = token_info.token_type;
    j["scope"] = token_info.scope;
    j["expires_in"] = token_info.expires_in;
    j["refresh_token"] = token_info.refresh_token;
    j["created_at"] = std::chrono::system_clock::to_time_t(token_info.created_at);

    std::ofstream file(token_file);
    file << j.dump(4);
}

bool SpotifyAPI::loadTokenFromFile()
{
    try
    {
        std::ifstream file(token_file);
        if (!file.is_open())
        {
            std::cout << "Could not open token file: " << token_file << std::endl;
            return false;
        }

        json j = json::parse(file);
        token_info.access_token = j["access_token"];
        token_info.token_type = j["token_type"];
        token_info.expires_in = j["expires_in"];
        token_info.scope = j["scope"];
        token_info.refresh_token = j["refresh_token"];
        token_info.created_at = std::chrono::system_clock::from_time_t(j["created_at"]);
        
        std::cout << "Successfully loaded token from file" << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "Error loading token file: " << e.what() << std::endl;
        return false;
    }
}

// @@@ end of token management @@@

// @@@ network logic @@@

size_t SpotifyAPI::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    userp->append((char *)contents, size * nmemb);
    return size * nmemb;
}

bool SpotifyAPI::refresh() {
    std::string response;
    struct curl_slist* headers = nullptr;
    std::string auth = client_id + ":" + client_secret;
    std::string auth_base64 = base64_encode(reinterpret_cast<const BYTE*>(auth.data()), auth.size());
    std::string auth_header = "Authorization: Basic " + auth_base64;

    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, auth_header.c_str());

    std::string token_request_data = "grant_type=refresh_token&refresh_token=" + token_info.refresh_token;

    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_POSTFIELDS, token_request_data.c_str());
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(SpotifyAPI::curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        last_error = "Failed to exchange refresh token for access token.";
        return false;
    }

    try {
        json j = json::parse(response);
        token_info.access_token = j["access_token"];
        token_info.token_type = j["token_type"];
        token_info.expires_in = j["expires_in"];
        token_info.scope = j["scope"];
        token_info.created_at = std::chrono::system_clock::now();

        saveTokenToFile();
        return true;
    } catch (...) {
        last_error = "Failed to parse refresh token response.";
        return false;
    }
}

bool SpotifyAPI::authenticate() {
    // generate authorization url
    std::string redirect_uri = "http://127.0.0.1:3000/callback";
    std::string auth_url = "https://accounts.spotify.com/authorize?response_type=code&client_id=" + client_id +
                           "&scope=user-read-playback-state" +
                           "&redirect_uri=" + redirect_uri +
                           "&state=" + random::generateRandomString(16);

    // print url
    std::cout << "Please open the following URL in your browser to authorize the application:\n" << auth_url << std::endl;

    // wait for the user to authorize and capture the authorization code
    std::string code;
    std::cout << "Enter the authorization code from the callback URL: ";
    std::cin >> code;

    // exchange the authorization code for an access token
    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    std::string token_request_data = "grant_type=authorization_code&code=" + code +
                                     "&redirect_uri=" + redirect_uri +
                                     "&client_id=" + client_id +
                                     "&client_secret=" + client_secret;

    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_POSTFIELDS, token_request_data.c_str());
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(SpotifyAPI::curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        last_error = "Failed to exchange authorization code for access token.";
        return false;
    }

    try {
        json j = json::parse(response);
        token_info.access_token = j["access_token"];
        token_info.token_type = j["token_type"];
        token_info.expires_in = j["expires_in"];
        token_info.scope = j["scope"];
        token_info.refresh_token = j["refresh_token"];
        token_info.created_at = std::chrono::system_clock::now();

        saveTokenToFile();
        return true;
    } catch (...) {
        last_error = "Failed to parse access token response.";
        return false;
    }
}

bool SpotifyAPI::getCurrentTrackRequest()
{
    std::string response;
    if (!SpotifyAPI::curl) {
        last_error = "cURL not initialized.";
        return false;
    }
    std::string token = getAccessToken();
    if (token.empty()) {
        last_error = "Failed to get access token.";
        return false;
    }
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
    
    // std::string additional_types = "?additional_types=track";
    char curl_errbuf[CURL_ERROR_SIZE];
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/currently-playing");
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(SpotifyAPI::curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(SpotifyAPI::curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        last_error = std::string("Request failed: ") + curl_errbuf;
        return false;
    }
    try {
        if (!response.empty()) {
            json j = json::parse(response);
            track_info.id = j["item"]["id"];
            track_info.progress_ms = j["progress_ms"];
            track_info.timestamp = j["timestamp"];
            current_track = j["item"]["name"];
        } else {
            current_track = "There is no music playing";
            track_info.id = "";
            track_info.progress_ms = 0;
            track_info.timestamp = 0;
        }
        return true;
    } catch (...) {
        last_error = "Failed to parse request.";
        return false;
    }
}

const std::string& SpotifyAPI::getLastError() const {
    return last_error;
}

// @@@ end of network logic @@@