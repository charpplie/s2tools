set("NAME" s2tools_auto)

set("SOURCES"
	"main.cpp"
)

add_executable(${NAME}
	${SOURCES}
)

target_link_libraries(${NAME} PRIVATE common_deps custom_deps)