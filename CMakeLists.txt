# CMakeList.txt : CMake project for NukeFocusStealer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

project (NukeFocusStealer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Output all binary files into one folder
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
 set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

configure_file(
 "${CMAKE_SOURCE_DIR}/src/version.h.in" "${BUILD_DIR}/src/version.h" @ONLY
 NEWLINE_STYLE LF
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(XFFA_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

if (MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

find_package(OpenGL REQUIRED)

if (OpenGL_FOUND)
 message(STATUS "OpenGL is found: ${OPENGL_LIBRARIES}")
else()
 message(STATUS "OpenGL is NOT found!")
endif()

set(SDL_AUDIO_ENABLED_BY_DEFAULT OFF)
set(SDL_JOYSTICK_ENABLED_BY_DEFAULT OFF)
set(SDL_HAPTIC_ENABLED_BY_DEFAULT OFF)
set(SDL_POWER_ENABLED_BY_DEFAULT OFF)
set(SDL_FILE_ENABLED_BY_DEFAULT OFF)
set(SDL_CPUINFO_ENABLED_BY_DEFAULT OFF)
set(SDL_DLOPEN_ENABLED_BY_DEFAULT OFF)
set(SDL_SENSOR_ENABLED_BY_DEFAULT OFF)
# Require policy set to new for fix CMake warning.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
# Really don't want DirectX renderer
set(SDL_DIRECTX OFF)
set(SDL_RENDER_D3D OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/import/SDL2" EXCLUDE_FROM_ALL)

add_subdirectory("${PROJECT_SOURCE_DIR}/projects/imgui")

# Not require since only include a header file
#add_subdirectory("${PROJECT_SOURCE_DIR}/import/simpleini" EXCLUDE_FROM_ALL)

include_directories(
 "${PROJECT_SOURCE_DIR}/import/simpleini"
)

file(GLOB_RECURSE FONTS "${PROJECT_SOURCE_DIR}/import/noto-fonts/*.ttf" "${PROJECT_SOURCE_DIR}/import/noto-fonts/*.otf")

file(GLOB_RECURSE LANGS "${PROJECT_SOURCE_DIR}/langs/*.ini")

# Disabled as not using icon. Part 1 / 2
#file(GLOB_RECURSE ICONS "${PROJECT_SOURCE_DIR}/src/icon/*.bmp")
#include("${PROJECT_SOURCE_DIR}/import/cmrc/CMakeRC.cmake")

# Add text files to this project for view purpose.
file(GLOB TEXTS
 "src/.clang-format"
 "LICENSE.txt"
 "README.md"
)

configure_file(
 "${CMAKE_SOURCE_DIR}/src/version.h.in" "${CMAKE_BINARY_DIR}/src/version.h" @ONLY
 NEWLINE_STYLE LF
)

# Add headers to this project's executable.
file(GLOB HEADERS
 "src/ui.hpp"
 "src/display.hpp"
 "src/translation.def"
 "src/translation.hpp"
 "${PROJECT_SOURCE_DIR}/import/imgui/backends/imgui_impl_opengl3.h"
 "${PROJECT_SOURCE_DIR}/import/imgui/backends/imgui_impl_sdl.h"
 "${PROJECT_SOURCE_DIR}/import/imgui/backends/imgui_impl_win32.h"
)

# Add source to this project's executable.
file(GLOB SOURCES
 "src/main.cpp"
 "src/ui.cpp"
 "src/display.cpp"
 "src/translation.cpp"
 "${PROJECT_SOURCE_DIR}/import/imgui/backends/imgui_impl_opengl3.cpp"
 "${PROJECT_SOURCE_DIR}/import/imgui/backends/imgui_impl_sdl.cpp"
 "${PROJECT_SOURCE_DIR}/import/imgui/backends/imgui_impl_win32.cpp"
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX headers FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX sources FILES ${SOURCES})

list(APPEND HEADERS "${CMAKE_BINARY_DIR}/src/version.h")

# Force build as GUI application
# Unknown what linux application will generate console window or not.
if (WIN32)
 set(BUILD_GUI WIN32)
elseif (APPLE)
 set(BUILD_GUI MACOSX_BUNDLE)
else()
 set(BUILD_GUI "")
endif()

add_executable(${PROJECT_NAME} ${BUILD_GUI} ${HEADERS} ${SOURCES} ${TEXTS})

target_include_directories(${PROJECT_NAME} PRIVATE "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

# Disabled as not using icon. Part 2 / 2
#cmrc_add_resource_library(${PROJECT_NAME}_rc NAMESPACE rc WHENCE "src/icon" ${ICONS})

# GCC needs a lib for std::filesystem
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(STD_FILESYSTEM_LIBRARY -lstdc++fs)
else()
  set(STD_FILESYSTEM_LIBRARY)
endif()

target_link_libraries(${PROJECT_NAME}
 PUBLIC
 ${OPENGL_LIBRARIES}
 SDL2-static
 SDL2main
 imgui
# ${PROJECT_NAME}_rc
 ${STD_FILESYSTEM_LIBRARY}
)

# Enforce default start up project
if (MSVC)
 # Configure startup project
 set_property(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
 set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
endif()

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)

add_custom_target(batch
 COMMAND ${CMAKE_COMMAND} -D "BUILD_DIR=${PROJECT_BINARY_DIR}" -P ${XFFA_ROOT_DIR}/projects/batch/batch.cmake
 WORKING_DIRECTORY ${XFFA_ROOT_DIR}
)

add_custom_command(TARGET ${PROJECT_NAME}
 COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/config/fonts
 COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONTS} $<TARGET_FILE_DIR:${PROJECT_NAME}>/config/fonts
 COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/config/langs
 COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LANGS} $<TARGET_FILE_DIR:${PROJECT_NAME}>/config/langs
 WORKING_DIRECTORY ${XFFA_ROOT_DIR}
)

add_dependencies(${PROJECT_NAME} batch)

# This project with third-party libraries
set_target_properties(${PROJECT_NAME}
 SDL2-static
 SDL2main
 imgui
 PROPERTIES FOLDER Third-Party
)

# Bind projects to this project
set_target_properties(${PROJECT_NAME}
#${PROJECT_NAME}_rc
 batch
 PROPERTIES FOLDER ${PROJECT_NAME}
)

install(DIRECTORY
 $<TARGET_FILE_DIR:${PROJECT_NAME}>/config
 DESTINATION bin
)

install(FILES
 LICENSE.txt
 README.md
 DESTINATION bin
)
