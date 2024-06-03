@echo off

@rem If you want the msvc dev console to be set up automatically, set built_init_vsdev
@rem to the path to your vcvarsall.bat file (in quotes if it contains spaces)
@rem along with the build archetype you want to use (e.g x64)
@rem Example: set build_init_vsdev="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if defined build_init_vsdev call :init_vsdev

@rem Set build_qt5_path to the location of your Qt5 install, if it isn't already in Path.
@rem Example: set build_qt5_path=C:\Qt\Qt-5.15.14\bin
if defined build_qt5_path call :set_qt5_path

@rem Define build_debug if you want to build the project in debug, otherwise it'll build in release.
@rem Alternatively, set build_type to debug or release.
@rem Example: set build_debug=1
if defined build_debug (
	set build_type=debug
) else (
	set build_type=release
)

goto :build


:init_vsdev
call %build_init_vsdev%

exit /b 0


:set_qt5_path
set Path=%build_qt5_path%;%Path%

exit /b 0


:build
echo:
echo Converting images...

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

echo:
echo Building...

qmake
nmake %build_type%

echo:
echo Deploying...
echo:

if not exist "%build_type%\deploy" mkdir %build_type%\deploy
copy %build_type%\aor.exe %build_type%\deploy
cd %build_type%\deploy
windeployqt aor.exe
cd ..\..

echo:
echo Build complete.
