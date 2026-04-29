set("NAME" s2tools_cli)

set("SOURCES"
	"main.cpp"
)

add_executable(${NAME}
	${SOURCES}
	${CMAKE_SOURCE_DIR}/res/res_cli.rc
)

target_link_libraries(${NAME} PRIVATE common_deps custom_deps)