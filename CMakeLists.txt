cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(CURL REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/src/include ${CURL_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(lyric-overlay 
    src/base64.hpp
    src/base64.cpp
    src/lyrics_api.cpp
    src/lyrics_api.hpp
    src/main.cpp
    src/random.cpp
    src/random.hpp
    src/spotify_api.hpp
    src/spotify_api.cpp
)
target_compile_features(lyric-overlay PRIVATE cxx_std_17)
target_link_libraries(lyric-overlay PRIVATE SFML::Graphics ${CURL_LIBRARIES} dwmapi)
