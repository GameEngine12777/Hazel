REM 关闭命令回显，也就是运行这个脚本的时候，命令行窗口不会把每一条命令都显示出来，输出更干净整洁。
@echo off

REM %~dp0： 这个批处理文件所在的文件夹路径（带最后的 \）
REM %0： 代表当前批处理脚本自身的路径（包含文件名）
REM ~dp0：是对 %0 的修饰符：
REM d： 表示“驱动器号”
REM p： 表示“路径（不含文件名）”
REM 切换到当前批处理文件所在目录的上一级目录，并把原始目录压栈
pushd %~dp0\..\

REM 运行 Premake，生成 Visual Studio 2022 的项目文件
call vendor\premake\bin\premake5.exe vs2022

popd
PAUSE

IF %ERRORLEVEL% NEQ 0 (
  REM 回到原来的目录
  popd

  REM 暂停脚本，让用户看到结果
  PAUSE
)