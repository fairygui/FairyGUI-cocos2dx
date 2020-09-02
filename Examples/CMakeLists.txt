#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.12)

set(APP_NAME FairyGUI_Examples)
project (${APP_NAME})

if(ANDROID)
    set(PLATFORM_SPECIFIC_SRC proj.android/jni/main.cpp)
    set(RES_PREFIX "/Resources")
elseif(WINDOWS)
    set(PLATFORM_SPECIFIC_SRC proj.win32/main.cpp)
    set(PLATFORM_SPECIFIC_HEADERS
            proj.win32/main.h
            proj.win32/resource.h
            )
    set(RES_PREFIX "")
elseif(IOS)
    set(PLATFORM_SPECIFIC_SRC
            proj.ios_mac/ios/main.m
            proj.ios_mac/ios/AppController.mm
            proj.ios_mac/ios/RootViewController.mm
            )

elseif(MACOSX OR APPLE)
    set(PLATFORM_SPECIFIC_SRC proj.ios_mac/mac/main.cpp)
    
    set(MACOSX_BUNDLE_BUNDLE_NAME "\${PRODUCT_NAME}")
    set(MACOSX_BUNDLE_INFO_PLIST proj.ios_mac/mac/Info.plist)
    set(MACOSX_BUNDLE_ICON_FILE Icon)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER org.cocos2dx.hellocpp)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING 1.0)
    set(MACOSX_BUNDLE_BUNDLE_VERSION 1)
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2017. All rights reserved.")

    file(GLOB_RECURSE RES_FILES Resources/*)
    cocos_mark_resources(FILES ${RES_FILES} BASEDIR Resources)
    file(GLOB_RECURSE RES_ICON proj.ios_mac/mac/Icon.icns)
    cocos_mark_resources(FILES ${RES_ICON} BASEDIR proj.ios_mac/mac)

    list(APPEND PLATFORM_SPECIFIC_SRC ${RES_FILES} ${RES_ICON})

elseif(LINUX)
    set(PLATFORM_SPECIFIC_SRC proj.linux/main.cpp)
    set(RES_PREFIX "/Resources")
else()
    message( FATAL_ERROR "Unsupported platform, CMake will exit" )

endif()

file(GLOB_RECURSE GAME_SRC Classes/*.c Classes/*.cc Classes/*.cpp)
file(GLOB_RECURSE GAME_HEADERS Classes/*.h Classes/*.hpp)
list(APPEND GAME_SRC ${PLATFORM_SPECIFIC_SRC})
list(APPEND GAME_HEADERS ${PLATFORM_SPECIFIC_HEADERS})

# add the executable
if(ANDROID)
    add_library(${APP_NAME} SHARED ${GAME_SRC} ${GAME_HEADERS})
    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        ADD_CUSTOM_COMMAND(TARGET ${APP_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} lib${APP_NAME}.so)
    ENDIF()
elseif(MSVC)
	add_executable(${APP_NAME} ${GAME_SRC} ${GAME_HEADERS} proj.win32/game.rc)
else()
    add_executable(${APP_NAME} ${GAME_SRC} ${GAME_HEADERS})
endif()

target_link_libraries(${APP_NAME} fairygui cocos2d)
target_include_directories(${APP_NAME} PRIVATE Classes)

set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${APP_NAME}")

if(APPLE)
    # output macOS/iOS .app
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE 1)
elseif(MSVC)
    # visual studio default is Console app, but we need Windows app
    set_property(TARGET ${APP_NAME} APPEND PROPERTY LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()
