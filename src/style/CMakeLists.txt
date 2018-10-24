add_library(adwaita_style )
target_sources(adwaita_style PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/helper.h"
                "${CMAKE_CURRENT_SOURCE_DIR}/adwaita_decoration_style.cpp"
			   PUBLIC
			   "${CMAKE_CURRENT_SOURCE_DIR}/adwaita_decoration_style.h"
			   "${CMAKE_CURRENT_SOURCE_DIR}/decoration_style.h")

target_compile_features(adwaita_style PUBLIC cxx_std_14)
target_link_libraries(adwaita_style PUBLIC Qt5::Core Qt5::Gui)

target_include_directories(adwaita_style PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
