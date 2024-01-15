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

此项目仅针对Explorer，如果您想将效果应用到全局，请看看我们的另一个项目[DWMBlurGlass](https://github.com/Maplespe/DWMBlurGlass).

## 兼容性
与 StartAllBack、OldNewExplorer等修改窗口样式的软件兼容.

也与第三方主题兼容

## 目录
- [预览](#预览)
- [如何使用](#如何使用)
- [配置文件](#配置文件)
- [其他](#其他)

## 预览
<details><summary><b>Windows 11</b></summary>

23H2 WinUI3
```ini
[config]
effect=1
clearBarBg=true
clearAddress=true
clearWinUIBg=true
[light]
r=255
g=255
b=255
a=200
....
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/012949.png)

Dark Mode
```ini
[config]
effect=2
clearBarBg=true
clearAddress=true
clearWinUIBg=true
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/013256.png)

22H2 XamlIslands
```ini
[config]
effect=1
clearBarBg=true
clearAddress=true
clearWinUIBg=true
[light]
r=255
g=255
b=255
a=200
....
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/152834.png)

```ini
[config]
effect=1
clearBarBg=true
clearAddress=true
clearWinUIBg=false
[light]
r=255
g=255
b=255
a=200
....
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/152929.png)

</details>

<details><summary><b>Windows 10</b></summary>

```ini
[config]
effect=1
clearBarBg=true
clearAddress=true
clearWinUIBg=false
[light]
r=222
g=222
b=222
a=200
```
![image](https://github.com/Maplespe/ExplorerBlurMica/blob/main/screenshot/230909.png)

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
#清除地址栏背景颜色
clearAddress=true
#清除滚动条背景颜色
#(注意:由于系统滚动条本身不透明 因此为了去除背景色 滚动条是由本程序自绘的 它可能和系统样式有所差别)
clearBarBg=true
#清除Windows11文件资源管理器的WinUI或XamlIslands部分的工具栏背景色
clearWinUIBg=true
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

此项目是基于枫の美化工具箱独立出来的组件 [MToolBox](https://winmoes.com/tools/12948.html).
