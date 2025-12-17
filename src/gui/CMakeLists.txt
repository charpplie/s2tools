set("NAME" s2tools_gui)

set("SOURCES"
	"main.cpp"
	"Renderer.cpp"
	"UI.cpp"
)

set("HEADERS"
	"Renderer.h"
	"UI.h"
)

add_executable(${NAME}
	${SOURCES}
	${HEADERS}
)

set_target_properties(${NAME} PROPERTIES
	LINK_FLAGS "/SUBSYSTEM:WINDOWS"
)

target_link_libraries(${NAME} PRIVATE common_deps custom_deps imgui)