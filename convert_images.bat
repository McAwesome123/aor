@echo off

rem Use /v for verbose output

if not exist "assets\img\items\sil" mkdir assets\img\items\sil

for %%I in (%CD%\assets\img\items\*.png) do (
	magick %%I -colorspace RGB -fill "#000000" -colorize 100 %CD%\assets\img\items\sil\%%~nxI
	if "%1"=="/v" echo %%I -^> %CD%\assets\img\items\sil\%%~nxI
)

if "%1"=="/v" echo:
if "%1"=="/v" echo ----------------------------------------
if "%1"=="/v" echo:

echo ^<!DOCTYPE RCC^>^<RCC version="1.0"^>^<qresource^> > images.qrc
if "%1"=="/v" echo ^<!DOCTYPE RCC^>^<RCC version="1.0"^>^<qresource^>
for %%I in (%CD%\assets\img\items\*.png) do (
	echo ^<file^>assets/img/items/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/img/items/%%~nxI^</file^>
)
for %%I in (%CD%\assets\img\items\sil\*.png) do (
	echo ^<file^>assets/img/items/sil/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/img/items/sil/%%~nxI^</file^>
)
for %%I in (%CD%\assets\img\icons\*.png) do (
	echo ^<file^>assets/img/icons/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/img/icons/%%~nxI^</file^>
)
for %%I in (%CD%\assets\img\lk\*.png) do (
	echo ^<file^>assets/img/lk/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/img/lk/%%~nxI^</file^>
)
for %%I in (%CD%\assets\img\style\*.png) do (
	echo ^<file^>assets/img/style/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/img/style/%%~nxI^</file^>
)
for %%I in (%CD%\assets\img\map\*.png) do (
	echo ^<file^>assets/img/map/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/img/map/%%~nxI^</file^>
)
for %%I in (%CD%\assets\wav\*.wav) do (
	echo ^<file^>assets/wav/%%~nxI^</file^> >> images.qrc
	if "%1"=="/v" echo ^<file^>assets/wav/%%~nxI^</file^>
)

echo ^</qresource^>^</RCC^> >> images.qrc
if "%1"=="/v" echo ^</qresource^>^</RCC^>
