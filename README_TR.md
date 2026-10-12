# ExplorerBlurMica
Windows 10 ve 11 için Dosya Gezginine Bulanık, Akrilik veya Mika arka plan efekti ekleyin

给文件资源管理器添加背景模糊效果或Acrylic、Mica效果 适用于win10和win11
#
| [中文](/README_ZH.md) | [English](/README.md) | [Türkçe](/README_TR.md)
Bu proje [LGNU V3 lisansı](/COPYING.LESSER) kullanmaktadır.

[![Lisans](https://img.shields.io/github/license/Maplespe/ExplorerBlurMica.svg)](https://www.gnu.org/licenses/lgpl-3.0.en.html)
[![Github Tüm Yayınlar](https://img.shields.io/github/downloads/Maplespe/ExplorerBlurMica/total.svg)](https://github.com/Maplespe/ExplorerBlurMica/releases)
[![GitHub yayını](https://img.shields.io/github/release/Maplespe/ExplorerBlurMica.svg)](https://github.com/Maplespe/ExplorerBlurMica/releases/latest)
<img src="https://img.shields.io/badge/language-c++-F34B7D.svg"/>
<img src="https://img.shields.io/github/last-commit/Maplespe/ExplorerBlurMica.svg"/>  

## Efektler
* Bulanık, Akrilik ve Mika efektleri mevcuttur.
* Özel karışım renkleri mevcuttur.
* Açık/Koyu mod ile uyumlu.

Bu proje sadece Dosya Gezgini içindir. Eğer efekti küresel olarak uygulamak isterseniz diğer projemiz olan [DWMBlurGlass](https://github.com/Maplespe/DWMBlurGlass)'a bir göz atın.

## Uyumluluk
StartAllBack, OldNewExplorer ve diğer pencere stillerini düzenleyen yazılımlarla uyumludur.

Ayrıca üçüncü parti temalarla da uyumludur.

## Katalog
- [Genel bakış](#overview)
- [Nasıl kullanılır](#how-to-use)
- [Yapılandırma](#config)
- [Diğer](#other)

## Genel bakış
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

Koyu Mod
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

## Nasıl kullanılır

### Yükle
1. Derlenmiş program arşivini [Yayın](https://github.com/Maplespe/ExplorerBlurMica/releases) sayfasından indirin.
2. "`C:\Program Files`" gibi bir dizinde arşivden çıkarın.
3. "`register.cmd`" dosyasını yönetici olarak çalıştırın.
4. Efektler için Dosya Gezgini penceresini yeniden açın.

Komut İstemi komutu: `regsvr32 "dizininiz/ExplorerBlurMica.dll"`

### Kaldır
1. "`uninstall.cmd`" dosyasını yönetici olarak çalıştırın.
2. Geri kalan dosyaları silin.

Komut İstemi komutu: `regsvr32 /u "dizininiz/ExplorerBlurMica.dll"`

#
Not: Eğer Dosya Gezginini çökerten bir şey olursa, Dosya Gezginini açmak ve programı kaldırmak için `ESC` tuşunu basılı tutun.

## Yapılandırma
``` ini
[config]
#Etki türü 0=Bulanık 1=Akrilik 2=Mika 3=Bulanık(Açık) 4=MikaAlt
#Bulanık efekti yalnızca Windows 11 22H2'ye kadar mevcuttur. Bulanık (Açık) hem Windows 10 hem de Windows 11'de
#mevcuttur. Mika yalnızca Windows 11'de mevcuttur.
effect=1
#Adres çubuğunun arka planını kaldır.
clearAddress=true
#Kaydırma çubuğunun arka planını kaldır.
#(Not: Sistem kaydırma çubuğunun kendi kaldırılamayan bir arka plan rengine sahip olduğundan,
# bu seçenek açıldığında, kaydırma çubuğu program tarafından çizilir ve stil sistemden farklı olabilir).
clearBarBg=true
#Araç çubuğu arka plan rengini Windows 11'in WinUI veya XamlIslands bölümünden kaldır.
clearWinUIBg=true
#TreeView ve DUIView arasında bölünmüş çizgiyi göster.
showLine=true
[light]
#Sistem renk düzeni Açık moddaki renktir.
#Arka plan karışım renginin #RGBA bileşeni
r=220
g=220
b=220
a=160
[dark]
#Sistem renk düzeni Koyu moddaki renktir.
r=0
g=0
b=0
a=120
```

Değişiklikten sonra yapılandırmayı kaydedin ve etkili olması için Dosya Gezgini penceresini yeniden açın.

## Diğer
Grafik kullanıcı arayüzü Python'da çalışmaktadır.
[minhook](https://github.com/m417z/minhook) ve [customtkinter](https://github.com/TomSchimansky/CustomTkinter) öğelerine bağlıdır.

Bu proje [MToolBox](https://winmoes.com/tools/12948.html) içindeki kodu temel alan bağımsız bir bileşendir.
