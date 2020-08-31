FairyGUI Cocos2dx SDK
====

A flexible UI framework for Cocos2dx, working with the FREE professional Game UI Editor: FairyGUI Editor.  
Download the editor from here: [www.fairygui.com](https://www.fairygui.com/)  

FairyGUI编辑器操作简单，使用习惯与Adobe系列软件保持一致，策划和美术设计师可以轻松上手。在编辑器即可组合各种复杂UI组件，以及为UI设计动画效果，无需编写任何代码。可一键导出到Unity，Cocos2dx, Starling, Egret, LayaAir, Pixi, Haxe, Flash等多个主流应用和游戏平台。

下载UI编辑器：[www.fairygui.com](https://www.fairygui.com/)

Get Started
====

目录结构：
- libfairygui 这里是fairygui库的源码。
- Examples 这里是示例的源码。
  - UIProject 这里是示例用到的UI界面的源码，使用FairyGUI编辑器打开。

cocos2dx源码需要改动一处地方才能通过编译，打开2d/CCLabel.h，大约在672行，为updateBMFontScale函数打上virtual修饰符。即
  ```
    virtual void updateBMFontScale();
  ```
  因为这个方法里有强制字体对象指针为FontFnt类型的代码，但我们不使用FontFnt（FontFnt只支持从外部文件中载入配置，更糟糕的是BMFontConfiguration是定义在cpp里的。）, 所以需要重写这个方法。

#### 使用Examples/proj.platform文件下的工程文件
请下载3.x版本cocos2dx命名成cocos2d放在Example根目录。

#### 使用cocos2dx的cmake生成工程
在cocos2dx的CMakeLists.txt文件末尾添加:
```
add_subdirectory(/youpath/FairyGUI-cocos2dx/libfairygui ${ENGINE_BINARY_PATH}/FairyGUI)
add_subdirectory(/youpath/FairyGUI-cocos2dx/Examples ${ENGINE_BINARY_PATH}/FairyGUI_Examples)
```
生成的可执行文件在bin/FairyGUI_Examples目录，需要自己复制Examples/Resources目录过去，windows还要复制依赖库（可以从cocos2dx生成的cpp-test目录复制dll）

Learn
====

[教程](https://www.fairygui.com/docs/guide/index.html)  

License
====

MIT
