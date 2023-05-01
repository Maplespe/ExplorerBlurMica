# ExplorerBlurMica
Add background Blur effect or Acrylic or Mica effect to explorer for win10 and win11

给文件资源管理器添加背景模糊效果或Acrylic、Mica效果 适用于win10和win11
#
| [中文](/README_ZH.md) | [English](/README.md) |
This project uses [LGNU V3 license](/COPYING.LESSER).

## Effects
* Blur or Acrylic, Mica effects are available.
* Custom blend colors are available.

## Compatibility
Compatible with StartAllBack, OldNewExplorer and other software that modifies window styles.

It also seems to be compatible with third-party themes!

There is currently a conflict with [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts) (because the same text layer rendering technology is used)

## Catalog
- [Overview](#overview)
- [How to use](#how-to-use)
- [Config](#config)
- [Other](#other)

## Overview
<details><summary><b>Windows 11</b></summary>

```ini
[config]
effect=1
showLine=false
[blend]
r=200
g=200
b=200
a=10
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/204426.png)

Dark Mode
```ini
[config]
effect=2
showLine=true
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/22h2mica.jpg)

*Blur effect is only available before 22H2
```ini
[config]
effect=0
showLine=true
[blend]
r=255
g=255
b=255
a=160
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/e0rgb255a160.jpg)
</details>

<details><summary><b>Windows 10</b></summary>

```ini
[config]
effect=1
smallBorder=false
showLine=false
darkRibbon=true
[blend]
r=220
g=220
b=220
a=120
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/201555.png)

```ini
[config]
effect=1
smallBorder=false
showLine=true
darkRibbon=true
[blend]
r=220
g=220
b=220
a=120
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/201720.png)

```ini
[config]
effect=1
smallBorder=false
showLine=false
darkRibbon=true
[blend]
r=27
g=179
b=129
a=50
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/204114.png)

```ini
[config]
effect=0
smallBorder=true
showLine=false
darkRibbon=true
[blend]
r=220
g=220
b=220
a=120
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/203646.png)

</details>

## How to use

### Install
1. Download the compiled program archive from the [Release](https://github.com/Maplespe/ExplorerBlurMica/releases) page.
2. Unzip it to a location such as "`C:\Program Files`".
3. Run "`register.cmd`" as administrator.
4. Reopen the Explorer window to take effect.

cmd: `regsvr32 "you path/ExplorerBlurMica.dll"`

### Uninstall
1. Run "`uninstall.cmd`" as administrator.
2. Delete the remaining files.

cmd: `regsvr32 /u "you path/ExplorerBlurMica.dll"`

#
Note: If something happens that crashes Explorer, press and hold the `ESC` key to open Explorer and uninstall the program.

## Config
``` ini
[config]
#Effect type 0=blur 1=acrylic(Mica is only available for win11 Blur effect is only available before 22H2)
effect=1
#This is to solve the problem that Blur effect under Windows 10 will make the window shadow also be blurred.
#If you use the Blur effect you can set it to true to alleviate this.
smallBorder=false
#This option specifies whether to show a separator line between the TreeView and the preview panel in the main view.
showLine=false
#Ribbon text colors and backgrounds are rendered incorrectly in Windows 10 Light mode.
#This option allows you to set the ribbon to dark mode to alleviate this problem.
darkRibbon=true
[blend]
#RGBA component of background blend color
r=255
g=255
b=255
a=160
```

Save the configuration after modification and reopen the File Explorer window to take effect.

## Other
Dependent on [minhook](https://github.com/m417z/minhook).

This project is an independent component based on the code in the [MyToolBox](https://winmoes.com/tools/12948.html).
