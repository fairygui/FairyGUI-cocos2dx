cmake_minimum_required(VERSION 3.12)

project(fairygui)

# file list
file(GLOB_RECURSE FAIRYGUI_SRC Classes/*.c Classes/*.cc Classes/*.cpp)
file(GLOB_RECURSE FAIRYGUI_HEADER Classes/*.h Classes/*.hpp)

add_library(fairygui ${FAIRYGUI_SRC} ${FAIRYGUI_HEADER})

target_link_libraries(fairygui PRIVATE cocos2d)
target_include_directories(fairygui PUBLIC Classes)
