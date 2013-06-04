@echo off

mkdir ..\..\Temp\Doc\Editor

echo ==== Generating the Doxygen documentation ====
..\..\Tools\Doxygen\doxygen.exe Editor.dox

echo ==== Generating the CHM file from the HTML directory ====
..\..\Tools\HTMLHelpCompiler\Compiler\hhc.exe ..\..\Temp\Doc\Editor\html\index.hhp

echo ==== Deleting the HTML temporary files ====
rmdir /S /Q ..\..\Temp\Doc\Editor

pause