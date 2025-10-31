cmake_minimum_required(VERSION 3.16)
project(MineCPP VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

if(WIN32)
    add_compile_definitions(_USE_MATH_DEFINES NOMINMAX)
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})

if(APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-shift-op-parentheses -Wno-tautological-constant-out-of-range-compare -Wno-parentheses -Wno-unqualified-std-cast-call)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

find_package(glfw3 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)

if(WIN32)
    find_package(Vorbis REQUIRED)
    set(VORBIS_LIBRARIES Vorbis::vorbisfile Vorbis::vorbis)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(VORBIS REQUIRED vorbisfile)
endif()

if(APPLE)
    pkg_check_modules(MESA REQUIRED glu)
    target_include_directories(${PROJECT_NAME} PRIVATE ${MESA_INCLUDE_DIRS} ${VORBIS_INCLUDE_DIRS})
    target_link_directories(${PROJECT_NAME} PRIVATE ${MESA_LIBRARY_DIRS} ${VORBIS_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ZLIB::ZLIB ${MESA_LIBRARIES} ${VORBIS_LIBRARIES})
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ZLIB::ZLIB opengl32 glu32 ${VORBIS_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ZLIB::ZLIB OpenGL::OpenGL GLU ${VORBIS_LIBRARIES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

file(COPY "${PROJECT_SOURCE_DIR}/resources" DESTINATION ${CMAKE_BINARY_DIR})
