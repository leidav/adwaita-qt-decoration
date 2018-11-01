add_library(adwaita SHARED main.cpp)
target_sources(adwaita PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/adwaita_decoration.h"
	           "${CMAKE_CURRENT_SOURCE_DIR}/adwaita_decoration.cpp")

target_compile_features(adwaita PRIVATE cxx_std_14)

target_link_libraries(adwaita PRIVATE adwaita_style Qt5::Core Qt5::Gui Qt5::WaylandClient)
target_include_directories(adwaita PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
${Qt5WaylandClient_PRIVATE_INCLUDE_DIRS})
target_include_directories(adwaita PRIVATE ${PRIVATE_QT_HEADER_DIR})

install(TARGETS adwaita
	    DESTINATION lib/qt/plugins/wayland-decoration-client)
