 cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

project("s2tools" LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
	sourcepp
	GIT_REPOSITORY https://github.com/craftablescience/sourcepp.git
	GIT_TAG 08dcf881b9aefd52008546eaa5682abf133fe9b9
)

FetchContent_MakeAvailable(sourcepp)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

set("IMGUI_SRC_FILES"
	"src/thirdparty/imgui/imgui.cpp"
	"src/thirdparty/imgui/imgui_demo.cpp"
	"src/thirdparty/imgui/imgui_draw.cpp"
	"src/thirdparty/imgui/imgui_impl_dx11.cpp"
	"src/thirdparty/imgui/imgui_impl_win32.cpp"
	"src/thirdparty/imgui/imgui_tables.cpp"
	"src/thirdparty/imgui/imgui_widgets.cpp"
)

set("IMGUI_HEADER_FILES"
	"src/include/imgui/imconfig.h"
	"src/include/imgui/imgui.h"
	"src/include/imgui/imgui_impl_dx11.h"
	"src/include/imgui/imgui_impl_win32.h"
	"src/include/imgui/imgui_internal.h"
	"src/include/imgui/imstb_rectpack.h"
	"src/include/imgui/imstb_textedit.h"
	"src/include/imgui/imstb_truetype.h"
)

set("S2_SRC_FILES"
	"src/main.cpp"
	"src/App.cpp"
	"src/Renderer.cpp"
	"src/UI.cpp"
)

set("S2_HEADER_FILES"
	"src/pch.h"
	"src/App.h"
	"src/Renderer.h"
	"src/UI.h"
)

add_executable(${PROJECT_NAME}   
	"${S2_SRC_FILES}"
	"${S2_HEADER_FILES}"

	"${IMGUI_SRC_FILES}"
	"${IMGUI_HEADER_FILES}"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

set_target_properties(${PROJECT_NAME} PROPERTIES
	LINK_FLAGS "/SUBSYSTEM:WINDOWS"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	sourcepp::steampp
	sourcepp::vpkpp
	OpenSSL::Crypto
	CURL::libcurl
)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src/include/" "${PROJECT_SOURCE_DIR}/src/include/imgui/")