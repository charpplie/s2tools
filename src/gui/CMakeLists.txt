set(NAME s2tools_gui)

set(SOURCES
	"main.cpp"
	"Renderer.cpp"
	"UI.cpp"
	"TaskManager.cpp"
	"AppearanceSettings.cpp"
	"tabs/Mods.cpp"
	"tabs/Randomizer.cpp"
	"tabs/Changer.cpp"
	"tabs/Tools.cpp"
	"tabs/Settings.cpp"
)

set(HEADERS
	"Renderer.h"
	"UI.h"
	"TaskManager.h"
	"AppearanceSettings.h"
	"tabs/Mods.h"
	"tabs/Randomizer.h"
	"tabs/Changer.h"
	"tabs/Tools.h"
	"tabs/Settings.h"
)

add_executable(${NAME}
	${SOURCES}
	${HEADERS}

	${CMAKE_SOURCE_DIR}/res/res_gui.rc
)

set_target_properties(${NAME} PROPERTIES
	LINK_FLAGS "/SUBSYSTEM:WINDOWS"
)

target_link_libraries(${NAME} PRIVATE common_deps custom_deps imgui)