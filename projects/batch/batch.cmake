# CMakeList.txt : CMake project for OracleInterpreter, include source and define
# project specific logic here.
#

find_package (Git)
if (GIT_FOUND)
 execute_process(
  COMMAND git describe --always --tags --first-parent --dirty
  OUTPUT_VARIABLE _GIT_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_VARIABLE _GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 message(DEBUG "Git version (pre): " ${_GIT_VERSION})
 if (NOT _GIT_VERSION EQUAL _GIT_HASH)
  string(FIND ${_GIT_VERSION} "-dirty" IS_DIRTY)
 else()
  set(IS_DIRTY 1)
 endif()
 if (NOT ${IS_DIRTY} EQUAL -1)
  unset(_GIT_VERSION)
  set(_GIT_VERSION "0.0.0")
  string(APPEND _GIT_HASH "-dirty")
 endif()
else()
 set(_GIT_VERSION "unknown")
endif()

 message("Git version: " ${_GIT_VERSION})

 message("Git hash: " ${_GIT_HASH})

configure_file(
 "${CMAKE_SOURCE_DIR}/src/version.h.in" "${BUILD_DIR}/src/version.h" @ONLY
 NEWLINE_STYLE LF
)
