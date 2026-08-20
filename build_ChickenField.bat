@echo off 

set CC=wcc386
set EXIT=../bin/ChickenField/obj

rem set COMPILATION_ARGS= -bm /6r -fp6 -ms -om -op -ot -oz -zkl -zdp -zi -ot  -ol -oo -e600
set COMPILATION_ARGS= /6r -fp6 -e600 -i="lefa/include" -ecc

cd SRC
echo.
echo.
echo ======== MAIN.c
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "ChickenField/main.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "ChickenField/camera/camera.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "ChickenField/util/util.c" -fo="%EXIT%/" ^
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
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/render/api_manager/gl_manager/gl_manager.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo  ======LODEPNG
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/3rdparty/lodepng/lodepng.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo GLAD_OPENGL COMPILATIOn
echo.
echo.
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "lefa/c_src/3rdparty/glad/OpenGL/glad_gl.c" -fo="%EXIT%/" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.




     cd ..
echo.
echo.
@wlink name ChickenField.exe ^
          file bin/ChickenField/obj/main.obj, ^
               bin/ChickenField/obj/util.obj, ^
               bin/ChickenField/obj/windowSystem.obj, ^
               bin/ChickenField/obj/input_manager.obj, ^
               bin/ChickenField/obj/gl_manager.obj, ^
               bin/ChickenField/obj/lodepng.obj, ^
               bin/ChickenField/obj/glad_gl.obj ^
          Library opengl32.lib, ^
               lib/dinput.lib, ^
               lib/dinput8.lib, ^
               lib/dxguid.lib, ^
               lib/pthreadWC2.lib, ^
               lib/zlib.lib, ^
               lib/libiconv.lib, ^
               lib/lehidjoy.lib
echo.
echo.


move ChickenField.exe bin/ChickenField/












pause 