cmake_minimum_required(VERSION 3.1)

if(POLICY CMP0022)
    cmake_policy(SET CMP0022 NEW)
endif()

project(fairygui)

# file list

set(FAIRYGUI_SRC   Classes/GSlider.cpp
  Classes/GLoader.cpp
  Classes/GTextInput.cpp
  Classes/tween/TweenManager.cpp
  Classes/tween/TweenValue.cpp
  Classes/tween/GTweener.cpp
  Classes/tween/EaseManager.cpp
  Classes/tween/TweenPropType.cpp
  Classes/tween/GTween.cpp
  Classes/GGroup.cpp
  Classes/Margin.cpp
  Classes/Transition.cpp
  Classes/DragDropManager.cpp
  Classes/ScrollPane.cpp
  Classes/GRoot.cpp
  Classes/UIPackage.cpp
  Classes/treeview/TreeNode.cpp
  Classes/treeview/TreeView.cpp
  Classes/Window.cpp
  Classes/PackageItem.cpp
  Classes/GObjectPool.cpp
  Classes/utils/ByteBuffer.cpp
  Classes/utils/WeakPtr.cpp
  Classes/utils/UBBParser.cpp
  Classes/utils/ToolSet.cpp
  Classes/GLabel.cpp
  Classes/UIObjectFactory.cpp
  Classes/controller_action/ChangePageAction.cpp
  Classes/controller_action/ControllerAction.cpp
  Classes/controller_action/PlayTransitionAction.cpp
  Classes/GObject.cpp
  Classes/GComboBox.cpp
  Classes/GComponent.cpp
  Classes/GRichTextField.cpp
  Classes/UIConfig.cpp
  Classes/PopupMenu.cpp
  Classes/GGraph.cpp
  Classes/GTextField.cpp
  Classes/GImage.cpp
  Classes/display/FUILabel.cpp
  Classes/display/BitmapFont.cpp
  Classes/display/FUIContainer.cpp
  Classes/display/FUIRichText.cpp
  Classes/display/FUIInput.cpp
  Classes/display/TextFormat.cpp
  Classes/display/FUISprite.cpp
  Classes/Controller.cpp
  Classes/Relations.cpp
  Classes/TranslationHelper.cpp
  Classes/GProgressBar.cpp
  Classes/gears/GearText.cpp
  Classes/gears/GearAnimation.cpp
  Classes/gears/GearBase.cpp
  Classes/gears/GearXY.cpp
  Classes/gears/GearColor.cpp
  Classes/gears/GearDisplay.cpp
  Classes/gears/GearSize.cpp
  Classes/gears/GearLook.cpp
  Classes/gears/GearIcon.cpp
  Classes/GScrollBar.cpp
  Classes/event/EventContext.cpp
  Classes/event/UIEventDispatcher.cpp
  Classes/event/InputEvent.cpp
  Classes/event/InputProcessor.cpp
  Classes/event/HitTest.cpp
  Classes/GMovieClip.cpp
  Classes/GList.cpp
  Classes/GButton.cpp
  Classes/RelationItem.cpp
  )



set(FAIRYGUI_HEADER Classes/GTextField.h
    Classes/GGroup.h
    Classes/GList.h
    Classes/tween/EaseManager.h
    Classes/tween/TweenManager.h
    Classes/tween/GTween.h
    Classes/tween/GTweener.h
    Classes/tween/EaseType.h
    Classes/tween/TweenPropType.h
    Classes/tween/TweenValue.h
    Classes/UIPackage.h
    Classes/Margin.h
    Classes/RelationItem.h
    Classes/GScrollBar.h
    Classes/PopupMenu.h
    Classes/treeview/TreeNode.h
    Classes/treeview/TreeView.h
    Classes/TranslationHelper.h
    Classes/UIConfig.h
    Classes/Transition.h
    Classes/GRoot.h
    Classes/GTextInput.h
    Classes/GRichTextField.h
    Classes/Relations.h
    Classes/Window.h
    Classes/UIObjectFactory.h
    Classes/GMovieClip.h
    Classes/utils/ByteBuffer.h
    Classes/utils/ToolSet.h
    Classes/utils/UBBParser.h
    Classes/utils/WeakPtr.h
    Classes/GComponent.h
    Classes/FieldTypes.h
    Classes/GImage.h
    Classes/controller_action/ChangePageAction.h
    Classes/controller_action/ControllerAction.h
    Classes/controller_action/PlayTransitionAction.h
    Classes/Controller.h
    Classes/FairyGUI.h
    Classes/GLabel.h
    Classes/GComboBox.h
    Classes/PackageItem.h
    Classes/GObjectPool.h
    Classes/FairyGUIMacros.h
    Classes/GProgressBar.h
    Classes/display/TextFormat.h
    Classes/display/BitmapFont.h
    Classes/display/FUIRichText.h
    Classes/display/FUIContainer.h
    Classes/display/FUISprite.h
    Classes/display/FUILabel.h
    Classes/display/FUIInput.h
    Classes/GLoader.h
    Classes/GSlider.h
    Classes/gears/GearColor.h
    Classes/gears/GearIcon.h
    Classes/gears/GearDisplay.h
    Classes/gears/GearText.h
    Classes/gears/GearXY.h
    Classes/gears/GearSize.h
    Classes/gears/GearBase.h
    Classes/gears/GearAnimation.h
    Classes/gears/GearLook.h
    Classes/DragDropManager.h
    Classes/GButton.h
    Classes/event/HitTest.h
    Classes/event/UIEventType.h
    Classes/event/EventContext.h
    Classes/event/InputProcessor.h
    Classes/event/InputEvent.h
    Classes/event/UIEventDispatcher.h
    Classes/GGraph.h
    Classes/GObject.h
    Classes/ScrollPane.h 
    )

list(APPEND FAIRYGUI_SRC ${FAIRYGUI_HEADER})

include_directories(
        ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)


add_library(fairygui ${FAIRYGUI_SRC})


target_link_libraries(
    fairygui
    PRIVATE
    cocos2d
)


target_include_directories(fairygui
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Classes>
)
