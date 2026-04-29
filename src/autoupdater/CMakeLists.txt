set("NAME" s2tools_auto)

set("SOURCES"
	"main.cpp"
)

add_executable(${NAME}
	${SOURCES}
	${CMAKE_SOURCE_DIR}/res/res_auto.rc
)

target_link_libraries(${NAME} PRIVATE common_deps custom_deps)