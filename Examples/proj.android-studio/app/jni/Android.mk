LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)
$(call import-add-path,$(LOCAL_PATH)/../../../../)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/BasicsScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/DemoScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/GuideScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/HitTestScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/JoystickModule.cpp \
                   $(LOCAL_PATH)/../../../Classes/JoystickScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/LoopListScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/MailItem.cpp \
                   $(LOCAL_PATH)/../../../Classes/MenuScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/ModalWaitingScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/PullToRefreshScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/TransitionDemoScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/VirtualListScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/BagScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/BagWindow.cpp \
                   $(LOCAL_PATH)/../../../Classes/EmojiParser.cpp \
                   $(LOCAL_PATH)/../../../Classes/ChatScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/ListEffectScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/ScrollPaneScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/TreeViewScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/Window1.cpp \
                   $(LOCAL_PATH)/../../../Classes/Window2.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2dx_static \
                    fairygui_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,libfairygui)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
