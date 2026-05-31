@echo off
echo ===================================
echo   BUILD TETRIS - Chon phien ban
echo ===================================
echo   1. Build tetris_v2.exe (v2)
echo   2. Build tetris_v3.exe (v3 - Arcade)
echo   3. Build ca hai
echo ===================================
set /p choice="Chon (1/2/3): "

if "%choice%"=="1" goto buildv2
if "%choice%"=="2" goto buildv3
if "%choice%"=="3" goto buildall
goto buildv3

:buildv2
echo.
echo [Building v2...]
g++ tetris_v2.cpp -o tetris_v2.exe ^
  -I".\raylib\raylib-5.5_win64_mingw-w64\include" ^
  -L".\raylib\raylib-5.5_win64_mingw-w64\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel%==0 ( echo [OK] v2 build thanh cong! && .\tetris_v2.exe ) else echo [LOI] Build v2 that bai!
goto end

:buildv3
echo.
echo [Building v3 - Arcade Classic...]
g++ tetris_v3.cpp -o tetris_v3.exe ^
  -I".\raylib\raylib-5.5_win64_mingw-w64\include" ^
  -L".\raylib\raylib-5.5_win64_mingw-w64\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel%==0 ( echo [OK] v3 build thanh cong! && .\tetris_v3.exe ) else echo [LOI] Build v3 that bai!
goto end

:buildall
call :buildv2_silent
call :buildv3_silent
echo.
echo [OK] Ca hai ban da duoc build!
goto end

:buildv2_silent
g++ tetris_v2.cpp -o tetris_v2.exe ^
  -I".\raylib\raylib-5.5_win64_mingw-w64\include" ^
  -L".\raylib\raylib-5.5_win64_mingw-w64\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm 2>nul
exit /b

:buildv3_silent
g++ tetris_v3.cpp -o tetris_v3.exe ^
  -I".\raylib\raylib-5.5_win64_mingw-w64\include" ^
  -L".\raylib\raylib-5.5_win64_mingw-w64\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm 2>nul
exit /b

:end
pause
