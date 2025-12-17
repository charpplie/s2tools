cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project("s2tools" LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
	sourcepp
	GIT_REPOSITORY https://github.com/craftablescience/sourcepp.git
	GIT_TAG 08dcf881b9aefd52008546eaa5682abf133fe9b9
)

FetchContent_MakeAvailable(sourcepp)

find_package(OpenSSL REQUIRED)
#find_package(CURL REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(common_deps INTERFACE)

target_include_directories(common_deps INTERFACE
	"${PROJECT_SOURCE_DIR}/src/public/"
    "${PROJECT_SOURCE_DIR}/src/thirdparty/include/" 
)

target_link_libraries(common_deps INTERFACE
    sourcepp::steampp
    sourcepp::vpkpp
    OpenSSL::Crypto
    #CURL::libcurl
)

add_subdirectory("src/appframework")

add_library(custom_deps INTERFACE)

target_link_libraries(custom_deps INTERFACE
	appframework
)

add_subdirectory("src/autoupdater")
add_subdirectory("src/cli")

add_subdirectory("src/thirdparty/imgui")
add_subdirectory("src/gui")