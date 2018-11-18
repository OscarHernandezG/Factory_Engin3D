# Factory_Engin3D
Factory engin3D is a 3D game engine createt by Òscar Hernàndez and Aleix Gabarró. Two students of CITM, UPC in Terrassa. This code is written in C++.

- Óscar Hernández [Github account](https://github.com/OscarHernandezG)
- Aleix Gabarró [Github account](https://github.com/aleixgab)
- Factory Engin3D [Github repository](https://github.com/OscarHernandezG/Factory_Engin3D)

## Features

We use an Octree to acelerate the drawing prosess of static GameObjects.
You can Save and Load scenes from the options menu.
You can use Ctrl+Z to un-do the movements of the GameObjects
You can add new GameObjects (empy GameObject) from the scene window.
You can add new Components from the Inspector window.
You can edit the components of a GameObject from the Inspector window.
You can remove components from the Inspector window.
You can load assets from the Assets window.
You can enter GameMode using the Play, Pause and Tick buttons.


## Inputs & Shortcuts

#### Camera controls
- Right Click: Rotate camera
  - WASD: Move camera
  - Q: Move down camera
  - E: Move up camera
- Alt
  - Left click: Orbit Object
  - Right click: Zoom
- F: Focus to the object
- Wheel: Zoom
- Shift: Duplicate movement speed

- Mouse wheel click: Select GameObject

- 1: Debug draw
- 3: Activate camera culling
- 0: Create empty random GameObjct
- C: Select camera component (To edit components from play mode)


#### Save and load
- Ctrl+S: Save
- Ctrl+L: Load

#### Open and close windows:
- Ctrl+A: About window
- Ctrl+C: Configuration window
- Ctrl+D: Demo window
- Ctrl+E: Example window
- Ctrl+M: MathGeoLib window
- Ctrl+R: Random number window
- Ctrl+T: Inspector window
- Ctrl+GRAVE: Console window
- Ctrl+H: Hierarchy window
- Ctrl+X: Close all windows


## External Libreries
- ImGui
-	SDL
- OPENGL
- MathGeoLib
- PCG
- Parson
- Assimp
- DevIL

## MIT License

Copyright (c) 2018 Óscar Hernández

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
