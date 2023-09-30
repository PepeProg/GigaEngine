cmake_minimum_required(VERSION 3.26)
project(engine)

add_subdirectory(gainput)

set(CMAKE_CXX_STANDARD 17)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-find/")

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)

set(LIBS
        ${GLFW3_LIBRARY}
        ${OPENGL_LIBRARIES}
        gainput
)

target_link_libraries(${PROJECT_NAME} "${LIBS}")
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/gainput/lib/include")
