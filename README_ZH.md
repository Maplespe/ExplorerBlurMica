# ExplorerBlurMica
Add background Blur effect or Acrylic or Mica effect to explorer for win10 and win11

给文件资源管理器添加背景模糊效果或Acrylic、Mica效果 适用于win10和win11
#
| [中文](/README_ZH.md) | [English](/README.md) |
本项目使用 [LGNU V3 license](/COPYING.LESSER).

[![license](https://img.shields.io/github/license/Maplespe/ExplorerBlurMica.svg)](https://www.gnu.org/licenses/lgpl-3.0.en.html)
[![Github All Releases](https://img.shields.io/github/downloads/Maplespe/ExplorerBlurMica/total.svg)](https://github.com/Maplespe/ExplorerBlurMica/releases)
[![GitHub release](https://img.shields.io/github/release/Maplespe/ExplorerBlurMica.svg)](https://github.com/Maplespe/ExplorerBlurMica/releases/latest)
<img src="https://img.shields.io/badge/language-c++-F34B7D.svg"/>
<img src="https://img.shields.io/github/last-commit/Maplespe/ExplorerBlurMica.svg"/>  

## 效果
* 可选Blur、Acrylic或者Mica效果
* 可自定义混合颜色
* 亮/暗 颜色模式自适应

## 兼容性
与 StartAllBack、OldNewExplorer等修改窗口样式的软件兼容.

似乎还与第三方主题兼容!

## 目录
- [预览](#预览)
- [如何使用](#如何使用)
- [配置文件](#配置文件)
- [其他](#其他)

## 预览
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

*Blur效果仅在 22H2 之前版本系统有效
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

## 如何使用

### 安装
1. 从 [Release](https://github.com/Maplespe/ExplorerBlurMica/releases) 页面下载压缩文档
2. 解压到一个地方，例如 "`C:\Program Files`"
3. 以管理员身份运行 "`register.cmd`"
4. 程序打开文件资源管理器窗口即可生效

cmd: `regsvr32 "你的路径/ExplorerBlurMica.dll"`

### 卸载
1. 以管理员身份运行 "`uninstall.cmd`"
2. 删除剩余文件

cmd: `regsvr32 /u "你的路径/ExplorerBlurMica.dll"`

#
注意：如果出现文件资源管理器异常崩溃请按住`ESC`键打开资源管理器并卸载该程序。

## 配置文件
``` ini
[config]
#效果类型: 0=Blur 1=Acrylic 2=Mica 3=Blur(Clear)
#Blur仅在win11 22H2之前版本可用; Blur(Clear)在win10和win11都可用; Mica仅限win11可用
effect=1
#这是为了解决Windows 10下的模糊效果会使窗口的阴影也变得模糊的问题。
#如果你使用Blur效果，你可以把它设置为true来缓解这个问题。
smallBorder=false
#该选项指定是否在主视图的TreeView和预览面板之间显示一条分隔线。
showLine=false
#Ribbon的文本颜色和背景在Windows 10 Light模式下呈现不正确。
#这个选项允许你将Ribbon设置为Dark模式，以缓解这一问题。
darkRibbon=true
#清除地址栏背景颜色
#(注意:如果你使用了StartAllBack 请将"右侧经典搜索栏"选项关掉 否则它会覆盖本程序的效果)
clearAddress=true
#清除滚动条背景颜色
#(注意:由于系统滚动条本身不透明 因此为了去除背景色 滚动条是由本程序自绘的 它可能和系统样式有所差别)
clearBarBg=true
[light]
#系统颜色模式为Light(亮色)时的颜色
#RGBA 颜色分量
r=220
g=220
b=220
a=160
[dark]
#系统颜色模式为Dark(暗色)时的颜色
r=0
g=0
b=0
a=120

```

修改完保存文件重新打开文件资源管理器窗口即可生效。

## 其他
依赖： [minhook](https://github.com/m417z/minhook).

此项目是基于枫の美化工具箱独立出来的组件 [MyToolBox](https://winmoes.com/tools/12948.html).
