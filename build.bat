@echo off

rem Uncomment if you want this script to automatically call the vcvarsall.bat file.
rem Set it to the location of your vcvarsall.bat file.
rem call "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

rem Uncomment if your Qt5 install is not under Path. 
rem Set Qt5Path to the location of your Qt5 install.
rem set Qt5Path=D:\Qt\Qt-5.15.14\bin
rem set Path=%Qt5Path%;%Path%

if not exist "assets\img\items\sil" mkdir assets\img\items\sil

for %%I in (%CD%\assets\img\items\*.png) do (
	magick %%I -colorspace RGB -fill "#000000" -colorize 100 %CD%\assets\img\items\sil\%%~nxI
)

echo ^<!DOCTYPE RCC^>^<RCC version="1.0"^>^<qresource^> > images.qrc
for %%I in (%CD%\assets\img\items\*.png) do (
	echo ^<file^>assets/img/items/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\items\sil\*.png) do (
	echo ^<file^>assets/img/items/sil/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\icons\*.png) do (
	echo ^<file^>assets/img/icons/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\lk\*.png) do (
	echo ^<file^>assets/img/lk/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\style\*.png) do (
	echo ^<file^>assets/img/style/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\map\*.png) do (
	echo ^<file^>assets/img/map/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\wav\*.wav) do (
	echo ^<file^>assets/wav/%%~nxI^</file^> >> images.qrc
)

echo ^</qresource^>^</RCC^> >> images.qrc

qmake
nmake release

if not exist "release\deploy" mkdir release\deploy
copy release\aor.exe release\deploy
cd release\deploy
windeployqt aor.exe
cd ..\..
