cmake_minimum_required (VERSION 3.8)

project (adwaita_decoration)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
	             "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	add_compile_options("-Wall")
endif()

set(PRIVATE_QT_HEADER_DIR CACHE STRING "Path to private qt wayland headers")
option(BUILD_TEST "Build test application" OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/wayland-decoration-client)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets WaylandClient REQUIRED)

add_subdirectory(src/style)
add_subdirectory(src/wayland)
add_subdirectory(src/headerbar)

if(BUILD_TEST)
	add_subdirectory(test)
endif()
