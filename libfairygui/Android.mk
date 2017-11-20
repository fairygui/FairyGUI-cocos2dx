LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := fairygui_static

LOCAL_MODULE_FILENAME := libfairygui

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := \
Classes/Controller.cpp \
Classes/DragDropManager.cpp \
Classes/GButton.cpp \
Classes/GComboBox.cpp \
Classes/GComponent.cpp \
Classes/GGraph.cpp \
Classes/GGroup.cpp \
Classes/GImage.cpp \
Classes/GLabel.cpp \
Classes/GList.cpp \
Classes/GLoader.cpp \
Classes/GMovieClip.cpp \
Classes/GObject.cpp \
Classes/GObjectPool.cpp \
Classes/GProgressBar.cpp \
Classes/GRichTextField.cpp \
Classes/GRoot.cpp \
Classes/GScrollBar.cpp \
Classes/GSlider.cpp \
Classes/GTextField.cpp \
Classes/GTextInput.cpp \
Classes/Margin.cpp \
Classes/PackageItem.cpp \
Classes/PopupMenu.cpp \
Classes/RelationItem.cpp \
Classes/Relations.cpp \
Classes/ScrollPane.cpp \
Classes/Transition.cpp \
Classes/UIConfig.cpp \
Classes/UIObjectFactory.cpp \
Classes/UIPackage.cpp \
Classes/Window.cpp \
Classes/display/Actions.cpp \
Classes/display/BitmapFont.cpp \
Classes/display/FUIContainer.cpp \
Classes/display/FUILabel.cpp \
Classes/display/FUIRichText.cpp \
Classes/display/FUISprite.cpp \
Classes/event/EventContext.cpp \
Classes/event/HitTest.cpp \
Classes/event/InputEvent.cpp \
Classes/event/InputProcessor.cpp \
Classes/event/UIEventDispatcher.cpp \
Classes/gears/GearAnimation.cpp \
Classes/gears/GearBase.cpp \
Classes/gears/GearColor.cpp \
Classes/gears/GearDisplay.cpp \
Classes/gears/GearIcon.cpp \
Classes/gears/GearLook.cpp \
Classes/gears/GearSize.cpp \
Classes/gears/GearText.cpp \
Classes/gears/GearXY.cpp \
Classes/utils/ByteArray.cpp \
Classes/utils/ToolSet.cpp \
Classes/utils/UBBParser.cpp \
Classes/utils/WeakPtr.cpp

LOCAL_STATIC_LIBRARIES := cocos2dx_static

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Classes

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Classes
                                 
include $(BUILD_STATIC_LIBRARY)
