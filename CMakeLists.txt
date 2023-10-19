cmake_minimum_required(VERSION 3.26)

project(RME CXX)

set(CMAKE_CXX_STANDARD 23)

############################## ADD SOURCE FILES TO TARGET ##################################

set(LIBRARIES ${CMAKE_SOURCE_DIR}/deps/libraries) # SET PATH TO LIBRARIES

add_subdirectory(${LIBRARIES}/bgfx.cmake) # RENDER ENGINE CORE

add_subdirectory(${LIBRARIES}/assimp-master) # PA

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp
)

file(GLOB_RECURSE ASSETS_FILES
        ${CMAKE_SOURCE_DIR}/assets/*.sc)

############################## ADD SDL2 ##################################

############################## ADD BGFX #################################
bgfx_compile_shader_to_header(
        TYPE VERTEX
        SHADERS ${CMAKE_SOURCE_DIR}/assets/shaders/default/vs_shader.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/assets/shaders/default/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

bgfx_compile_shader_to_header(
        TYPE FRAGMENT
        SHADERS ${CMAKE_SOURCE_DIR}/assets/shaders/default/fs_shader.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/assets/shaders/default/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

find_package(SDL2 REQUIRED)

target_include_directories(RME PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/include/generated/shaders
)
############################## LINK LIBRARIES #################################

target_link_libraries(RME
PRIVATE
        bgfx
        bimg
        bx
        ${SDL2_LIBRARIES}
        assimp
)

