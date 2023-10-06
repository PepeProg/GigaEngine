cmake_minimum_required(VERSION 3.26)

project(RME CXX)

set(CMAKE_CXX_STANDARD 23)

############################## ADD SOURCE FILES TO TARGET ##################################

set(LIBRARIES ${CMAKE_SOURCE_DIR}/deps/libraries) # SET PATH TO LIBRARIES

set(BGFX_3RDPARTY ${LIBRARIES}/bgfx.cmake/bgfx/3rdparty)

add_subdirectory(${LIBRARIES}/bgfx.cmake) # RENDER ENGINE CORE

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp
#        ${BGFX_3RDPARTY}/*.c
        ${BGFX_3RDPARTY}/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        ${BGFX_3RDPARTY}/*.h
        ${BGFX_3RDPARTY}/*.hpp
)

file(GLOB_RECURSE ASSETS_FILES
        ${CMAKE_SOURCE_DIR}/assets/*.sc)

############################## ADD SDL2 ##################################

############################## ADD BGFX #################################
bgfx_compile_shader_to_header(
        TYPE VERTEX
        SHADERS ${CMAKE_SOURCE_DIR}/assets/shaders/cube/vs_shader.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/assets/shaders/cube/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

bgfx_compile_shader_to_header(
        TYPE FRAGMENT
        SHADERS ${CMAKE_SOURCE_DIR}/assets/shaders/cube/fs_shader.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/assets/shaders/cube/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

find_package(SDL2 REQUIRED)

target_include_directories(RME PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/include/generated/shaders
        ${LIBRARIES}/bgfx.cmake
        ${BGFX_3RDPARTY}/cgltf
        ${BGFX_3RDPARTY}/directx-headers
        ${BGFX_3RDPARTY}/dear-imgui
        ${BGFX_3RDPARTY}/fcpp
        ${BGFX_3RDPARTY}/glsl-optimizer
        ${BGFX_3RDPARTY}/glslang
        ${BGFX_3RDPARTY}/khronos
        ${BGFX_3RDPARTY}/meshoptimizer
        ${BGFX_3RDPARTY}/native_app_glue
        ${BGFX_3RDPARTY}/renderdoc
        ${BGFX_3RDPARTY}/sdf
        ${BGFX_3RDPARTY}/spirv-cross
        ${BGFX_3RDPARTY}/spirv-headers
        ${BGFX_3RDPARTY}/stb
        ${BGFX_3RDPARTY}/spirv-tools
        ${BGFX_3RDPARTY}/webgpu
)

############################## LINK LIBRARIES #################################

target_link_libraries(RME
PRIVATE
        bgfx
        bimg
        bx
        ${SDL2_LIBRARIES}

)

