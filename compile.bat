@echo off
echo compile.bat
C:/VulkanSDK/1.3.261.1/Bin/glslc.exe shaders/basic.vert -o shaders/vert.spv && C:/VulkanSDK/1.3.261.1/Bin/glslc.exe shaders/basic.frag -o shaders/frag.spv

REM Проверка кода возврата

if %errorlevel% equ 0 (
    echo Command executed successfully
) else (
    echo Command failed with error level %errorlevel%
)