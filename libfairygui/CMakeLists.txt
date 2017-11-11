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
cmake_policy(SET CMP0017 NEW)

cmake_minimum_required(VERSION 3.1)

set(APP_NAME MyGame)
project (${APP_NAME})

set(COCOS2D_ROOT ${CMAKE_SOURCE_DIR}/../Examples/cocos2d)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${COCOS2D_ROOT}/cmake/Modules/")
include(CocosBuildHelpers)


# Some macro definitions
if(WINDOWS)
  if(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS (-D_USRDLL -D_EXPORT_DLL_ -D_USEGUIDLL -D_USREXDLL -D_USRSTUDIODLL)
  else()
    ADD_DEFINITIONS (-DCC_STATIC)
  endif()

  ADD_DEFINITIONS (-DCOCOS2DXWIN32_EXPORTS -D_WINDOWS -DWIN32 -D_WIN32)
  set(PLATFORM_FOLDER win32)
elseif(MACOSX OR APPLE)
  ADD_DEFINITIONS (-DCC_TARGET_OS_MAC)
  ADD_DEFINITIONS (-DUSE_FILE32API)
  set(PLATFORM_FOLDER mac)
elseif(LINUX)
  ADD_DEFINITIONS(-DLINUX)
  set(PLATFORM_FOLDER linux)
elseif(ANDROID)
  ADD_DEFINITIONS (-DUSE_FILE32API)
  set(PLATFORM_FOLDER android)
else()
  message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()


# Compiler options
if(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt /NODEFAULTLIB:libcmt")
  else()
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt")
  endif()
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
                  -wd4251 -wd4244 -wd4334 -wd4005 -wd4820 -wd4710
                  -wd4514 -wd4056 -wd4996 -wd4099)
else()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-DCOCOS2D_DEBUG=1)
  endif()
  set(CMAKE_C_FLAGS_DEBUG "-g -Wall")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations -Wno-reorder")
  if(CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif(MSVC)

# libcocos2d
set(BUILD_CPP_EMPTY_TEST OFF CACHE BOOL "turn off build cpp-empty-test")
set(BUILD_CPP_TESTS OFF CACHE BOOL "turn off build cpp-tests")
set(BUILD_LUA_LIBS OFF CACHE BOOL "turn off build lua related targets")
set(BUILD_JS_LIBS OFF CACHE BOOL "turn off build js related targets")
add_subdirectory(${COCOS2D_ROOT})

include_directories(
	/usr/local/include/GLFW
	/usr/include/GLFW
	${COCOS2D_ROOT}
	${COCOS2D_ROOT}/cocos
	${COCOS2D_ROOT}/external
	${COCOS2D_ROOT}/cocos/platform
	${COCOS2D_ROOT}/cocos/audio/include/
	Classes
)
if ( WINDOWS )
    include_directories(
            ${COCOS2D_ROOT}/external/glfw3/include/${PLATFORM_FOLDER}
            ${COCOS2D_ROOT}/external/${PLATFORM_FOLDER}-specific/gles/include/OGLES
    )
elseif ( MACOSX OR APPLE )
    include_directories(
            ${COCOS2D_ROOT}/external/glfw3/include/mac
    )
endif ()

set(GAME_SRC
        ${PLATFORM_SPECIFIC_SRC}
        Classes/Controller.cpp
        Classes/DragDropManager.cpp
        Classes/GButton.cpp
        Classes/GComboBox.cpp
        Classes/GComponent.cpp
        Classes/GGraph.cpp
        Classes/GGroup.cpp
        Classes/GImage.cpp
        Classes/GLabel.cpp
        Classes/GList.cpp
        Classes/GLoader.cpp
        Classes/GMovieClip.cpp
        Classes/GObject.cpp
        Classes/GObjectPool.cpp
        Classes/GProgressBar.cpp
        Classes/GRichTextField.cpp
        Classes/GRoot.cpp
        Classes/GScrollBar.cpp
        Classes/GSlider.cpp
        Classes/GTextField.cpp
        Classes/GTextInput.cpp
        Classes/Margin.cpp
        Classes/PackageItem.cpp
        Classes/PopupMenu.cpp
        Classes/RelationItem.cpp
        Classes/Relations.cpp
        Classes/ScrollPane.cpp
        Classes/Transition.cpp
        Classes/UIConfig.cpp
        Classes/UIObjectFactory.cpp
        Classes/UIPackage.cpp
        Classes/Window.cpp
        Classes/display/Actions.cpp
        Classes/display/BitmapFont.cpp
        Classes/display/FUIContainer.cpp
        Classes/display/FUILabel.cpp
        Classes/display/FUIRichText.cpp
        Classes/display/FUISprite.cpp
        Classes/event/EventContext.cpp
        Classes/event/HitTest.cpp
        Classes/event/InputEvent.cpp
        Classes/event/InputProcessor.cpp
        Classes/event/UIEventDispatcher.cpp
        Classes/gears/GearAnimation.cpp
        Classes/gears/GearBase.cpp
        Classes/gears/GearColor.cpp
        Classes/gears/GearDisplay.cpp
        Classes/gears/GearIcon.cpp
        Classes/gears/GearLook.cpp
        Classes/gears/GearSize.cpp
        Classes/gears/GearText.cpp
        Classes/gears/GearXY.cpp
        Classes/utils/ByteArray.cpp
        Classes/utils/ToolSet.cpp
        Classes/utils/UBBParser.cpp
        )

set(GAME_HEADERS
        ${PLATFORM_SPECIFIC_HEADERS}
        Classes/Controller.h
        Classes/DragDropManager.h
        Classes/GButton.h
        Classes/GComboBox.h
        Classes/GComponent.h
        Classes/GGraph.h
        Classes/GGroup.h
        Classes/GImage.h
        Classes/GLabel.h
        Classes/GList.h
        Classes/GLoader.h
        Classes/GMovieClip.h
        Classes/GObject.h
        Classes/GObjectPool.h
        Classes/GProgressBar.h
        Classes/GRichTextField.h
        Classes/GRoot.h
        Classes/GScrollBar.h
        Classes/GSlider.h
        Classes/GTextField.h
        Classes/GTextInput.h
        Classes/Margin.h
        Classes/PackageItem.h
        Classes/PopupMenu.h
        Classes/RelationItem.h
        Classes/Relations.h
        Classes/ScrollPane.h
        Classes/Transition.h
        Classes/UIConfig.h
        Classes/UIObjectFactory.h
        Classes/UIPackage.h
        Classes/Window.h
        Classes/display/Actions.h
        Classes/display/BitmapFont.h
        Classes/display/FUIContainer.h
        Classes/display/FUILabel.h
        Classes/display/FUIRichText.h
        Classes/display/FUISprite.h
        Classes/event/EventContext.h
        Classes/event/HitTest.h
        Classes/event/InputEvent.h
        Classes/event/InputProcessor.h
        Classes/event/UIEventDispatcher.h
        Classes/gears/GearAnimation.h
        Classes/gears/GearBase.h
        Classes/gears/GearColor.h
        Classes/gears/GearDisplay.h
        Classes/gears/GearIcon.h
        Classes/gears/GearLook.h
        Classes/gears/GearSize.h
        Classes/gears/GearText.h
        Classes/gears/GearXY.h
        Classes/utils/ByteArray.h
        Classes/utils/ToolSet.h
        Classes/utils/UBBParser.h
        )

# add the executable
if(ANDROID)
    add_library(${APP_NAME} STATIC ${GAME_SRC} ${GAME_HEADERS})
    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        ADD_CUSTOM_COMMAND(TARGET ${APP_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} lib${APP_NAME}.so)
    ENDIF()
elseif(MSVC)
    add_library(${APP_NAME} STATIC ${GAME_SRC} ${GAME_HEADERS})
else()
    add_library(${APP_NAME} STATIC ${GAME_SRC} ${GAME_HEADERS})
endif()