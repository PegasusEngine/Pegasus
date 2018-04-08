## About Pegasus
![](https://github.com/PegasusEngine/Pegasus/raw/master/Doc/GitHub/wiki//editor.png)


Pegasus is a realtime computer generated graphics engine developed by an enthusiastic group of graphics programmers. Original developers are Karolyn Boulanger, Kleber Garcia and David Worsham.


Make sure to check the wiki for further documentation: [Pegasus wiki](https://github.com/PegasusEngine/Pegasus/wiki)


## Programmer Setup

1) Download and install visual studio 2015
2) Open the project's solution file Project/VS14/Pegasus.sln
3) Build the example demo TestApp1 and set this project as initial project.
4) Run from visual studio, and TestApp1 should be running.

## Editor Programmer Setup

1) Follow the instructions for the programmer setup.
2) Install QT Tools from the visual studio extensions: In visual studio, Under Tools select Extensions and Updates. Search and install Qt Visual Studio Tools extension
3) Once the qt tools are installed, we must point to our qt package inside the Pegasus project. Go to Qt Vs Tools in the upper options bar, select Qt Options
4) A dialog box will pop, create a new Qt installation, lets call it "PegasusQt" or whichever name youd like. Click Add by doing this, and point to the directory Lib/Qt/5.8 inside the project.
5) Below, in the drop box that says 'Default Qt / Win version', select "PegasusQt" or whichever custom name you chose for the installation.
6) Right click on the Editor project in the project navigation of visual studio and click on build soltion. This should trigger a build and we should be able to run / modify the editor.

## Building QT from scratch

Pegasus comes already with a precompiled dynamically linkable version of QT under the Lib/Qt/5.8 folder. So these build steps are unnecesary unless there is a desire to modify vanilla QT 5.8.
If we desire to build this from scratch, we can download the source at https://github.com/kecho/PegasusQt
This should be unnecesary unless there is a desire to modify QT from its originally 5.8 stage. On this case, store the custom symbols on the Lib/Qt/5.8<customversion> folder.
These steps are compatible with the LGPL license and all of its obligations. 
