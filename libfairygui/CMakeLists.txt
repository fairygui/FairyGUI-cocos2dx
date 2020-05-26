cmake_minimum_required(VERSION 3.1)

if(POLICY CMP0022)
    cmake_policy(SET CMP0022 NEW)
endif()

project(fairygui)

if(MSVC)
    add_compile_options(/w)
else()
    add_compile_options(-w)
endif()

# file list

file(GLOB FAIRYGUI_SRC
        Classes/*.c*
        Classes/controller_action/*.c*
        Classes/display/*.c*
        Classes/event/*.c*
        Classes/gears/*.c*
        Classes/tween/*.c*
        Classes/utils/*.c*
        )

add_library(${PROJECT_NAME} ${FAIRYGUI_SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE cocos2d)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Classes>
)
