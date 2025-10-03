cmake_minimum_required(VERSION 3.16)
project(MineCPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(glfw3 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED)

if(APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MESA REQUIRED glu)
    target_include_directories(${PROJECT_NAME} PRIVATE ${MESA_INCLUDE_DIRS})
    target_link_directories(${PROJECT_NAME} PRIVATE ${MESA_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ZLIB::ZLIB ${MESA_LIBRARIES} ${Boost_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ZLIB::ZLIB OpenGL GLU ${Boost_LIBRARIES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

file(GLOB TEXTURES "${PROJECT_SOURCE_DIR}/textures/*")
file(COPY ${TEXTURES} DESTINATION ${CMAKE_BINARY_DIR})
