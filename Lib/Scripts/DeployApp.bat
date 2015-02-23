@echo off
echo "######################################################"
echo "############     PEGASUS APP DEPLOYER    #############"
echo "######################################################"
echo "Target: %2"

echo "Copying runtime libs (fmod & dx11 compiler)"
copy %3\..\..\Lib\RuntimeLibs\%1\*.dll %4\%1\Applications\%2\.

If Not Exist %4\%1\Applications\%2\Imported mklink /J %4\%1\Applications\%2\Imported %3\..\..\Data\%2\Imported
