cmake_minimum_required(VERSION 3.26)

project(RME CXX)

set(CMAKE_CXX_STANDARD 17)

############################## SET VARIABLES ##################################
set(SOURCE_EXE src/main.cpp) # SET EXECUTABLE
set(LIBRARIES ${CMAKE_SOURCE_DIR}/deps/libraries) # SET PATH TO LIBRARIES

############################## ADD EXE && OPTIONS ########################################
add_executable(RME ${SOURCE_EXE})
set_target_properties(RME PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED YES)
target_compile_features(RME PRIVATE cxx_std_14)
set_target_properties(RME PROPERTIES
        FOLDER src
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
target_compile_options(RME PRIVATE -lSDL2 -lGL -lX11 -ldl -lpthread -lrt)
############################## ADD LIBRARIES ##################################
find_package(SDL2 REQUIRED)

target_include_directories(RME PRIVATE
        ${LIBRARIES}/DiligentCore
        ${LIBRARIES}/bgfx/include
        ${LIBRARIES}/bx/include
        ${LIBRARIES}/bimg/include
        ${SDL2_INCLUDE_DIRS}
)
add_subdirectory(${LIBRARIES}/bgfx.cmake) # RENDER ENGINE CORE

############################## LINK LIBRARIES #################################
target_link_libraries(RME
PRIVATE
        bgfx
        bimg
        bx
        glfw
        ${SDL2_LIBRARIES}
)

############################## COPY SHARED LIBRARIES FOR EXE ##################
#copy_required_dlls(RME)










## Add source files
#file(GLOB_RECURSE SOURCE_FILES
#        ${CMAKE_SOURCE_DIR}/src/*.c
#        ${CMAKE_SOURCE_DIR}/src/*.cpp)
#
## Add header files
#file(GLOB_RECURSE HEADER_FILES
#        ${CMAKE_SOURCE_DIR}/src/*.h
#        ${CMAKE_SOURCE_DIR}/src/*.hpp)
#
#add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
#
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-find/")
#
#find_package(OpenGL REQUIRED)
#find_package(GLFW3 REQUIRED)
#
#set(LIBS
#        ${GLFW3_LIBRARY}
#        ${OPENGL_LIBRARIES}
#        gainput
#        OpenAL
#)
#FetchContent_Declare(
#        OpenAL
#        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
#        GIT_TAG        3e1c6e4351e18a6c189afd08b63e44e81ed0ecf2
#)
#FetchContent_Declare(
#        gainput
#        GIT_REPOSITORY https://github.com/jkuhlmann/gainput.git
#        GIT_TAG        e21b15f0bc3dd3f1a745fe89a966a2457e940142
#)
#FetchContent_MakeAvailable(gainput openal)
#
#target_link_libraries(${PROJECT_NAME} "${LIBS}")
#target_include_directories(${PROJECT_NAME} PUBLIC "${gainput_SOURCE_DIR}/lib/include")
#target_include_directories(${PROJECT_NAME} PUBLIC "${openal_SOURCE_DIR}/include")
