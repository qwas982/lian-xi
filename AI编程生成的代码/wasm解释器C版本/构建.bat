@echo off
chcp 65001 >nul
REM 构建.bat - WebAssembly解释器构建脚本 (Windows)

echo ========================================
echo WebAssembly解释器 C89版本 构建脚本
echo ========================================
echo.

set 编译器=gcc
set 输出名=wasm解释器.exe
set 编译选项=-std=c89 -Wall -Wextra -pedantic -O2

REM 检查编译器
where %编译器% >nul 2>nul
if errorlevel 1 (
    echo 错误: 未找到GCC编译器
    echo 请安装MinGW或MSYS2,并确保gcc在PATH中
    exit /b 1
)

echo 使用编译器: %编译器%
echo 编译选项: %编译选项%
echo.

REM 源文件列表
set 源文件们=数据结构.c 工具函数.c 模块加载器.c 解释器核心.c 主程序.c

echo 编译源文件...
%编译器% %编译选项% -o %输出名% %源文件们%

if errorlevel 1 (
    echo.
    echo ========================================
    echo 构建失败!
    echo ========================================
    exit /b 1
)

echo.
echo ========================================
echo 构建成功!
echo 输出文件: %输出名%
echo ========================================
echo.
echo 使用方法:
echo   %输出名% --测试           运行内置测试
echo   %输出名% ^<wasm文件^>      加载并执行Wasm模块
echo   %输出名% --帮助           显示帮助信息
echo.

exit /b 0
