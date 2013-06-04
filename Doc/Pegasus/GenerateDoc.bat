@echo off

mkdir ..\..\Temp\Doc\Pegasus

echo ==== Generating the Doxygen documentation ====
..\..\Tools\Doxygen\doxygen.exe Pegasus.dox

echo ==== Generating the CHM file from the HTML directory ====
..\..\Tools\HTMLHelpCompiler\Compiler\hhc.exe ..\..\Temp\Doc\Pegasus\html\index.hhp

echo ==== Deleting the HTML temporary files ====
rmdir /S /Q ..\..\Temp\Doc\Pegasus

pause