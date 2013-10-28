Wire3D
======

Wire3D is a multiplatform 3D engine written in C++, featuring:

  * Platform independent rendering, supporting Wii (Homebrew) and PC (DirectX)
    * Drawcall batching of static and dynamic objects
    * Minimizing render state changes
    * Automatic displaylist (Wii) generation of static meshes
    * Sorting by transparency, material, state, depth or custom keys
  * Scene graph (optional) with a modular extendible design, including:
    * Hierarchical culling and controller system
    * Render state, light and effect handling
    * Custom nodes (lod, billboard, skybox, text, ...)
  * Vector, matrix, quaternion lib with Wii specific implementations
  * Object system and containers
  * Input system handling/emulating Wiimote and Nunchuk

  * [Unity3D exporter](http://code.google.com/p/wire3d/wiki/Unity3DExporter) (assets, scene graph, physics properties, ...)
  * Importer (xml, png, ttf et al.)
  * Bullet Physics engine integration

  * Project files for
    * Wii: Visual Studio 2005/2008, 2010, Programmer's Notepad 2 and Makefiles
    * PC: Visual Studio 2005/2008 and 2010
  * Includes [Sample](http://code.google.com/p/wire3d/wiki/Samples) programs that serve as tutorials

Check out [this guide](http://code.google.com/p/wire3d/wiki/GettingStarted) on how to setup the framework.
