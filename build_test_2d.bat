@echo off 

set CC=wcc386
set EXIT=../bin/test_demo_2D/obj
set GAME_SOURCE_DIR=test_demo_game_2d


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
@wcc386 %COMPILATION_ARGS% -i="lefa/include" "%GAME_SOURCE_DIR%/util/util.c" -fo="%EXIT%/" ^
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
echo  __________(RESOURCE)________________________________________
echo /____________________________________________________________\
     @wrc %GAME_SOURCE_DIR%/resources/pres.rc -r -fo="%EXIT%/pres.res"   ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo ______________________________________________________________
echo \____________________________________________________________/
echo.
echo.




     cd ..
echo.
echo.
@wlink option resource 'bin/test_demo_2d/obj/pres.res' name game_2D.exe ^
          file bin/test_demo_2d/obj/main.obj, ^
                bin/test_demo_2d/obj/camera.obj, ^
                bin/test_demo_2d/obj/util.obj, ^
                bin/test_demo_2d/obj/windowSystem.obj, ^
                bin/test_demo_2d/obj/input_manager.obj, ^
                bin/test_demo_2d/obj/gl_manager.obj, ^
                bin/test_demo_2d/obj/lodepng.obj, ^
                bin/test_demo_2d/obj/glad_gl.obj ^
          Library opengl32.lib, ^
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


move game_2D.exe bin/test_demo_2d/












pause 