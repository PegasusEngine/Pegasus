## About Pegasus
![](https://github.com/PegasusEngine/Pegasus/raw/master/Doc/GitHub/wiki//editor.png)

Pegasus is a realtime computer generated graphics engine developed by an enthusiastic group of graphics programmers. Original developers are Karolyn Boulanger, Kleber Garcia and David Worsham.

We decided to create Pegasus as an expression of our talent, experimentation and as a greater learning experience. The engine has evolved into something truly beautiful and we would like to share this technology (and its source) with the world.

Pegasus does not impose a rendering pipeline to the programmer, instead provides a simplified interface for the procedural generation of meshes / textures and optional script abstraction for low level graphics APIs. Pegasus also includes a timeline and usage of fmod to play music. The programmer has the choice to have a fully data driven demo, or a write it in pure c++ using the API. The greatest use case for Pegasus is to fabricate procedural based demos, The goal is to make a visual engine extensible enough so possibilities are endless.

Pegasus is currently implemented leveraging dx11, however this layer is abstracted already and there are opportunities to support other graphics APIs. The openGL 4 api implementation is a current WIP. Vulcan and dx12 would be desired.
Pegasus offers low level access (vertex / texures / shaders / compute) through a high level interface/language (BlockScript).

## Documentation topics

* [Programmer's Guide] (ProgrammersGuide)
* [Editor Overview](Editor)
* [BlockScript](BlockScript)
* [Timeline](Timeline)
   + [Scripting in the timeline](ScriptingInTimeline)
* [Assets](Assets)
* [The Graph System] (Graph System)
* [Meshes](Meshes)
* [Textures](Textures)
* [Render API] (RenderApi)
* [RenderSystems](RenderSystems)
