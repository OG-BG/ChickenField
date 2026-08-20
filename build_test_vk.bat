@echo off 

set CC=wcc386
set EXIT=../bin/test_demo_vk/obj
set EXIT2=bin/test_demo_vk/obj
set EXIT_BIN=bin/test_demo_vk/
set GAME_SOURCE_DIR=test_demo_vk
set BIN_NAME=demo_vk.exe

rem set COMPILATION_ARGS= -bm /6r -fp6 -ms -om -op -ot -oz -zkl -zdp -zi -ot  -ol -oo -e600
set COMPILATION_ARGS= /6r -fp6 -bm -e600 -i="lefa/include" -ecc

cd SRC
echo.
echo.
echo ======== MAIN.c
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "%GAME_SOURCE_DIR%/main.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "%GAME_SOURCE_DIR%/camera/camera.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/platforms/windows/windowSystem.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/platforms/windows/input_manager.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/render/api_manager/vk_manager/vk_manager.c" -fo="%EXIT%/" ^
-DVK_USE_PLATFORM_WIN32_KHR ^
-DVK_PROTOTYPES ^
-D_CRT_SECURE_NO_WARNINGS ^
-D_USE_MATH_DEFINES ^
-D_DEBUG ^
-D_WINDOWS ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo  ======LODEPNG
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/3rdparty/lodepng/lodepng.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo  __________(RESOURCE)________________________________________
echo /____________________________________________________________\
     @wrc %GAME_SOURCE_DIR%/resources/pres.rc -r -fo="%EXIT%/pres.res"   ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo ______________________________________________________________
echo \____________________________________________________________/
echo.
echo.

pause


     cd ..
echo.
echo.
@wlink option resource '%EXIT2%/pres.res' name demo_vk.exe ^
          file %EXIT2%/main.obj, ^
               %EXIT2%/camera.obj, ^
               %EXIT2%/windowSystem.obj, ^
               %EXIT2%/input_manager.obj, ^
               %EXIT2%/vk_manager.obj, ^
               %EXIT2%/lodepng.obj ^
          Library lib/vulkan-1.lib, ^
               lib/dinput.lib, ^
               lib/dinput8.lib, ^
               lib/dxguid.lib, ^
               lib/pthreadWC2.lib, ^
               lib/zlib.lib, ^
               lib/libiconv.lib, ^
               lib/lehidjoy.lib, ^
               ddk/hid.lib, ^
               setupapi.lib
echo.
echo.


move %BIN_NAME% %EXIT_BIN%












pause 