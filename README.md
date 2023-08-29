# ogl-particles

Particle simulation with OpenGL. The particles are solved on the CPU and rendered on the GPU.
Handles half a million particles on an AMD R9 290X GPU.

> **Note**:
A relatively recent GPU with fresh drivers is necessary to run this application. Support for OpenGL 4.4 core profile is required. 

## Controls

 + **[ LMB + Drag ]** &emsp;&emsp;Turn Camera
 + **[ Mouse Wheel ]** &emsp;&nbsp;Zoom in & out
 + **[ END ]** &emsp;&emsp;&emsp;&emsp;&emsp;&ensp;Quit

## How to Compile
Clone the repo:
```bash
git clone git://github.com/MisterRooster/ogl-particles.git
cd ogl-particles 
```

Then simply run CMake with your favourite IDE. Tested with Visual Studio 2022,
no working guarantee for other environments.
Example commands for VS 2022:
```bash
mkdir build
cd build
cmake -G "Visual Studio 17"..
```
-----
Copyright (c) 2023 MisterRooster ([github.com/MisterRooster](https://github.com/MisterRooster)). All rights reserved.  
Licensed under the MIT license. See [LICENSE](LICENSE) for full terms.
