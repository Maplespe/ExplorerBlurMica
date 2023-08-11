# ExplorerBlurMica
Add background Blur effect or Acrylic or Mica effect to explorer for win10 and win11

给文件资源管理器添加背景模糊效果或Acrylic、Mica效果 适用于win10和win11
#
| [中文](/README_ZH.md) | [English](/README.md) |
This project uses [LGNU V3 license](/COPYING.LESSER).

[![license](https://img.shields.io/github/license/Maplespe/ExplorerBlurMica.svg)](https://www.gnu.org/licenses/lgpl-3.0.en.html)
[![Github All Releases](https://img.shields.io/github/downloads/Maplespe/ExplorerBlurMica/total.svg)](https://github.com/Maplespe/ExplorerBlurMica/releases)
[![GitHub release](https://img.shields.io/github/release/Maplespe/ExplorerBlurMica.svg)](https://github.com/Maplespe/ExplorerBlurMica/releases/latest)
<img src="https://img.shields.io/badge/language-c++-F34B7D.svg"/>
<img src="https://img.shields.io/github/last-commit/Maplespe/ExplorerBlurMica.svg"/>  

## Effects
* Blur or Acrylic, Mica effects are available.
* Custom blend colors are available.
* Light/Dark Mode Adaptive.

## Compatibility
Compatible with StartAllBack, OldNewExplorer and other software that modifies window styles.

It also seems to be compatible with third-party themes!

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
clearAddress=false
[light]
r=200
g=200
b=200
a=10
....
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/204426.png)

```ini
[config]
effect=3
showLine=false
clearAddress=true
clearBarBg=true
....
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/011806.png)

Dark Mode
```ini
[config]
effect=2
showLine=true
clearAddress=false
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/22h2mica.jpg)

*Blur effect is only available before 22H2
```ini
[config]
effect=0
showLine=true
clearAddress=false
[light]
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
clearAddress=false
[light]
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
clearAddress=false
[light]
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
clearAddress=false
[light]
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
clearAddress=false
[light]
r=220
g=220
b=220
a=120
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/203646.png)

```ini
[config]
effect=1
smallBorder=true
showLine=false
darkRibbon=true
clearAddress=true
clearBarBg=true
[light]
r=220
g=220
b=220
a=120
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/003702.png)

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
#Effect type 0=Blur 1=Acrylic 2=Mica 3=Blur(Clear)
#Blur is only available until win11 22h2, Blur (Clear) is available in both win10 and win11, Mica is win11 only.
effect=1
#This is to solve the problem that Blur effect under Windows 10 will make the window shadow also be blurred.
#If you use the Blur effect you can set it to true to alleviate this.
smallBorder=false
#This option specifies whether to show a separator line between the TreeView and the preview panel in the main view.
showLine=false
#Ribbon text colors and backgrounds are rendered incorrectly in Windows 10 Light mode.
#This option allows you to set the ribbon to dark mode to alleviate this problem.
darkRibbon=true
#Clear the background of the address bar.
#(Note: If you use StartAllBack, you must close the "Classic search box", otherwise it overrides the effect of this program).
clearAddress=true
#Clear the background color of the scrollbar.
#(Note: Since the system scrollbar itself has a background color that cannot be removed,
# when this option is turned on, the scrollbar is drawn by the program and the style may be different from the system).
clearBarBg=true
[light]
#The system color scheme is the color in Light mode.
#RGBA component of background blend color
r=220
g=220
b=220
a=160
[dark]
#The system color scheme is the color in Dark mode.
r=0
g=0
b=0
a=120
```

Save the configuration after modification and reopen the File Explorer window to take effect.

## Other
Dependent on [minhook](https://github.com/m417z/minhook).

This project is an independent component based on the code in the [MyToolBox](https://winmoes.com/tools/12948.html).
