add_library(headerbar)
target_sources(headerbar PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/headerbar.cpp"
               PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/headerbar.h")

target_compile_features(headerbar PUBLIC cxx_std_14)

target_link_libraries(headerbar PUBLIC adwaita_style Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(headerbar PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
