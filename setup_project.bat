@echo off
:: 核心修复：将工作目录切换到脚本文件所在的文件夹 (D:\repo\plantVSzombies)
cd /d "%~dp0"

setlocal
title PVZ 项目构建工具

echo ======================================================
echo   PVZ Project Setup
echo ======================================================

:: 1. 检查并清理旧 build 文件夹
if exist build (
    echo [信息] 发现旧 build 文件夹，正在清理...
    rd /s /q build
)

:: 2. 创建 build 目录
mkdir build

:: 3. 运行 CMake 
:: -S . 代表当前目录 (即脚本所在的 D:\repo\plantVSzombies)
echo [信息] 正在调用 CMake 生成解决方案...
cmake -S . -B build -G "Visual Studio 17 2022" -A win32 -T v143 -Wno-deprecated

if %errorlevel% neq 0 (
    echo.
    echo ------------------------------------------------------
    echo [错误] 工程生成失败！
    echo ------------------------------------------------------
    pause
    exit /b
)

echo.
echo [成功] 工程已生成在 build 目录下 。
echo 请双击 build/PVZ.sln 并将 PVZ 设为启动项目 。
pause