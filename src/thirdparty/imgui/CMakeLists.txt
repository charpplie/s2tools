set(IMGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_widgets.cpp
)

set(IMGUI_SOURCES_WIN
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_impl_dx11.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_impl_win32.cpp
)

add_library(imgui
	STATIC
		${IMGUI_SOURCES}
		${IMGUI_SOURCES_WIN}
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include/)