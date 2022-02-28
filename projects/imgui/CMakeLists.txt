cmake_minimum_required (VERSION 3.8)
project(imgui LANGUAGES CXX)
# Since imgui doesn't have CMake, we'll make an interface project here.

if (MSVC)
 add_compile_definitions(
 _CRT_SECURE_NO_WARNINGS
 _CRT_NONSTDC_NO_DEPRECATE
 )
endif()

# Add any defines from imconfig.h file in here without need to edit import file directly.
add_compile_definitions(
 IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
 IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
)

file (GLOB HEADERS
 "${XFFA_ROOT_DIR}/import/imgui/imconfig.h"
 "${XFFA_ROOT_DIR}/import/imgui/imgui.h"
 "${XFFA_ROOT_DIR}/import/imgui/imgui_internal.h"
 "${XFFA_ROOT_DIR}/import/imgui/imstb_rectpack.h"
 "${XFFA_ROOT_DIR}/import/imgui/imstb_textedit.h"
 "${XFFA_ROOT_DIR}/import/imgui/imstb_truetype.h"
 "${XFFA_ROOT_DIR}/import/imgui/misc/cpp/imgui_stdlib.h"
)


file (GLOB SOURCES
 "${XFFA_ROOT_DIR}/import/imgui/imgui.cpp"
 "${XFFA_ROOT_DIR}/import/imgui/imgui_draw.cpp"
 "${XFFA_ROOT_DIR}/import/imgui/imgui_tables.cpp"
 "${XFFA_ROOT_DIR}/import/imgui/imgui_widgets.cpp"
 "${XFFA_ROOT_DIR}/import/imgui/misc/cpp/imgui_stdlib.cpp"
)

source_group(TREE ${XFFA_ROOT_DIR}/import/imgui PREFIX header FILES ${HEADERS})

source_group(TREE ${XFFA_ROOT_DIR}/import/imgui PREFIX source FILES ${SOURCES})

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
 PUBLIC "${XFFA_ROOT_DIR}/import/imgui"
)
