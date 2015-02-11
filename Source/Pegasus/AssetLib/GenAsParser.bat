rem Asset Script parser Generator
rem \author Kleber Garcia
rem \notes cd into the source folder, and run this batch file to autogenerate the lexer and parser for assetscript

@echo off
echo ###################################################
echo ###### Asset Script Parser Generator Script #######
echo ###################################################

set TOOLS=..\..\..\Tools
set BISON=%TOOLS%\FlexAndBison\Win32\win_bison.exe
set FLEX=%TOOLS%\FlexAndBison\Win32\win_flex.exe
set INCLUDES=..\..\..\Include\Pegasus\AssetLib
set PREFIX=AS_

%BISON% -p %PREFIX% -d -o as.parser.cpp as.y
move as.parser.hpp %INCLUDES%\.
%FLEX% -P %PREFIX% --header-file=as.lexer.hpp --wincompat -o as.lexer.cpp as.l
move as.lexer.hpp %INCLUDES%\.
