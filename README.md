# ExplorerBlurMica
Add background Blur effect or Acrylic or Mica effect to explorer for win10 and win11

给文件资源管理器添加背景模糊效果或Acrylic、Mica效果 适用于win10和win11

此项目使用[LGNU v3开源许可证](/COPYING.LESSER)(This project uses [LGNU V3 license](/COPYING.LESSER))

本项目依赖于[minhook](https://github.com/m417z/minhook)(Dependent on [minhook](https://github.com/m417z/minhook))
## 效果(Effects)
* 可选Blur或Acrylic、Mica
* 可以自定义背景混合颜色

#
* Optional blur or Acrylic or Mica
* You can customize the background blend color
## 目录
1. [预览 (Overview)](#预览-overview)
2. [使用方法 (How to use)](#使用方法-how-to-use)
3. [配置文件 (Config)](#配置文件-config)
4. [其他 (Other)](#其他-other)

## 预览 (Overview)
### Windows 11
```ini
effect=1
rgba 179 117 245 100
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/r179g117b245a100.jpg)

Dark Mode
```ini
effect=2
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/22h2mica.jpg)

Blur效果仅限22H2之前(Blur effect is only available before 22H2)
```ini
effect=0
rgba 255 255 255 160
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/e0rgb255a160.jpg)

### Windows 10
```ini
effect=1
rgba 80 155 214 100
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/172503.jpg)

```ini
effect=0
rgba 255 255 255 160
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/172643.jpg)

## 使用方法 (How to use)
从[Release](https://github.com/Maplespe/ExplorerBlurMica/releases)页面下载编译好的程序

解压后以管理员运行"`register.cmd`"

如果要卸载 以同样方式运行"`uninstall.cmd`"

也可以手动使用命令提示符注册或卸载
> `regsvr32 (/u) "你的路径/ExplorerBlurMica.dll"`

如果出现任何问题导致崩溃 可以按住`ESC`键再打开文件资源管理器窗口

#

From [Release](https://github.com/Maplespe/ExplorerBlurMica/releases)Page download the compiled program

After decompression, run '`register.cmd`' as an administrator

You can also manually use CMD to register or uninstall
> `regsvr32 (/u) "you path/ExplorerBlurMica.dll"`

If there is any problem causing a crash, you can press and hold the `ESC` key to open the explorer window
## 配置文件 (Config)
``` ini
[config]
#效果类型 0=Blur 1=Acrylic 2=Mica (Mica仅在win11可用 Blur效果仅在22H2之前可用)
#Effect type 0=blur 1=acrylic(Mica is only available for win11 Blur effect is only available before 22H2)
effect=1
#在windows10因为API会把窗口边框的阴影也模糊掉 程序默认会调整窗口为窄边框 如果你不需要 设置为false
#In windows10, because the API will blur the shadow of the window border, the program will adjust the window to a narrow border by default. If you don't need it, set it to false
smallBorder=true
[blend]
#背景混合颜色的RGBA分量
#RGBA component of background blend color
r=255
g=255
b=255
a=160
```
修改完并保存以后 重新打开文件资源管理器窗口即可生效

After modification and saving, reopen the explorer window to take effect

## 其他 (Other)
本项目是基于[枫の美化工具箱](https://winmoes.com/tools/12948.html)中的代码独立出来的组件

This project is an independent component based on the code in the [MyToolBox](https://winmoes.com/tools/12948.html)
