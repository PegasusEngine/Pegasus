rem Block Script parser Generator
rem \author Kleber Garcia
rem \notes cd into the source folder, and run this batch file to autogenerate the lexer and parser for blockscript

@echo off
echo ###################################################
echo ###### Block Script Parser Generator Script #######
echo ###################################################

set TOOLS=..\..\..\Tools
set BISON=%TOOLS%\FlexAndBison\Win32\win_bison.exe
set FLEX=%TOOLS%\FlexAndBison\Win32\win_flex.exe
set INCLUDES=..\..\..\Include\Pegasus\BlockScript
set PREFIX=BS_

%BISON% -p %PREFIX% -d -o bs.parser.cpp bs.y
move bs.parser.hpp %INCLUDES%\.
%FLEX% -P %PREFIX% --wincompat -o bs.lexer.cpp bs.l
