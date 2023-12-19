cmake_minimum_required(VERSION 3.7)
project(ppg)

set(PKG_NAMES libenet)
set(CXXFLAGS -Wall -Wextra -Wpedantic -ggdb)
set(BOOST_PKGS )

file(GLOB_RECURSE SRC_LIST src/*.cxx)
file(GLOB_RECURSE MDLS_LIST src/*.cxxm)

if (DEFINED PKG_NAMES)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(PKGS REQUIRED $ENV{PKG_NAMES} ${PKG_NAMES})
  include_directories(${PKGS_INCLUDE_DIRS})
  list(APPEND LIBS_LIST ${PKGS_LIBRARIES})
endif ()

if (DEFINED BOOST_PKGS)
  find_package(Boost REQUIRED ${BOOST_PKGS})
  include_directories(${Boost_INCLUDE_DIRS})
  list(APPEND LIBS_LIST ${Boost_LIBRARIES})
endif ()

### capnp
find_package(CapnProto REQUIRED)
file(GLOB_RECURSE CAPNP_SCHEMAS ${CMAKE_SOURCE_DIR}/capnp/**.capnp)
if (NOT "${CAPNP_SCHEMAS}" STREQUAL "")
  capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${CAPNP_SCHEMAS})
endif ()
include_directories(${CAPNP})
include_directories(${CMAKE_BINARY_DIR})
list(APPEND SRC_LIST ${CAPNP_SRCS})
list(APPEND LIBS_LIST ${CAPNP_LIBRARIES})

### godot
list(APPEND LIBS_LIST
    ${CMAKE_SOURCE_DIR}/godot-cpp/build/bin/libgodot-cpp.linux.release.64.a)   
include_directories(godot-cpp/build/gen/include)
include_directories(godot-cpp/include)
include_directories(godot-cpp/gdextension)

include_directories(include)
include_directories(include/common)
set(CMAKE_CXX_STANDARD 23)
add_compile_options($ENV{CXXFLAGS} ${CXXFLAGS})

# add_executable(${PROJECT_NAME} ${SRC_LIST})
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})
