set("NAME" appframework)

set("SOURCES"
	"appframework.cpp"
)

set("HEADERS"
	"${CMAKE_CURRENT_SOURCE_DIR}/../public/appframework.h"
)

add_library(${NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${NAME} PRIVATE common_deps) 