REM 关闭命令回显，也就是运行这个脚本的时候，命令行窗口不会把每一条命令都显示出来，输出更干净整洁。
@echo off

REM 切换到上一级目录
pushd ..\

REM 运行 Premake，生成 Visual Studio 2022 的项目文件
call Extras\premake\premake5.exe vs2022

IF %ERRORLEVEL% NEQ 0 (
  REM 回到原来的目录
  popd

  REM 暂停脚本，让用户看到结果
  PAUSE
)