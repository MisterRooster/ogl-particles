![Preview](preview.gif)

# ogl-particles

Particle simulation with OpenGL and Imgui as user interface.
The particles are solved on the CPU and rendered on the GPU.
SSE & AVX intrinsics are used to speed up vector calculations.
Handles half a million particles on an AMD R9 290X GPU.

> **Note**:
A relatively recent GPU with fresh drivers is necessary to run this application. Support for OpenGL 4.4 core profile is required. 

## Controls

 + **[ LMB + Drag ]** &emsp;&emsp;Turn Camera
 + **[ RMB + Drag ]** &emsp;&emsp;Zoom in & out
 + **[ ESCAPE ]** &emsp;&emsp;&emsp;&emsp;&nbsp;Quit

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

## 3rd Party

**ogl-particles** uses 3rd party software, they are located in the `./dependencies` folder as submodules and built with the program. A copy of each is included in the repository when built.
Structure and description of 3rd parts libraries and related copyrights and licenses:

 - dependecies/**imGui** → Dear ImGui
[https://github.com/ocornut/imgui](https://github.com/ocornut/imgui) - [*License*](https://github.com/ocornut/imgui/blob/v1.89.8-docking/LICENSE.txt)<br>
uses **docking** release of **imGui** - [*tag (v1.89.8-docking)*](https://github.com/ocornut/imgui/tree/v1.89.8-docking)

- dependecies/**glfw** → A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
 [https://github.com/glfw/glfw](https://github.com/glfw/glfw) - [*License*](https://github.com/glfw/glfw/blob/3.3.8/LICENSE.md)<br>
uses version 3.3.8 - [*tag (3.3.8)*](https://github.com/glfw/glfw/tree/3.3.8)

- dependencies/**glew** → GL/GLES/EGL/GLX/WGL Loader
[https://github.com/nigels-com/glew](https://github.com/nigels-com/glew) - [*License*](https://github.com/nigels-com/glew/tree/glew-2.2.0)<br>
uses a slightly altered version compatible with *CMake* named [**glew-cmake**](https://github.com/Perlmint/glew-cmake) - [*tag (glew-cmake-2.2.0)*](https://github.com/Perlmint/glew-cmake/tree/glew-cmake-2.2.0)

- dependencies/**glm** → OpenGL vector and matrix math with SIMD
[https://github.com/g-truc/glm](https://github.com/g-truc/glm) - [*License*](https://github.com/g-truc/glm/blob/47585fde0c49fa77a2bf2fb1d2ead06999fd4b6e/copying.txt)<br>
uses version 9.9.8 - [*commit (47585fd)*](https://github.com/g-truc/glm/tree/47585fde0c49fa77a2bf2fb1d2ead06999fd4b6e)

- src/thirdparty/**stb_image.h** → Single file image loader
[https://github.com/nothings/stb](https://github.com/nothings/stb) - [*License*](https://github.com/nothings/stb/blob/5736b15f7ea0ffb08dd38af21067c314d6a3aae9/LICENSE)<br>
uses version 2.28 - [*commit (5736b15)](https://github.com/nothings/stb/tree/5736b15f7ea0ffb08dd38af21067c314d6a3aae9)

The following resources are used:

 - data/**icons** → Application icon made by [@Eucalyp](https://www.flaticon.com/authors/eucalyp) on
 [Flaticon](https://www.flaticon.com/free-icons/element)<br>
 [https://www.flaticon.com/free-icon/molecule_3201368](https://www.flaticon.com/free-icon/molecule_3201368)

 - data/**sprites** → Sprites from Kenney Particle Pack mady by [@Kenney](https://opengameart.org/users/kenney) on
 [OpenGameArt](https://opengameart.org/)<br>
 [https://opengameart.org/content/particle-pack-80-sprites](https://opengameart.org/content/particle-pack-80-sprites)

- data/**fonts** → Regular Font and Icon Font by Microsoft and Google <br>
[**codicon.ttf**](https://github.com/microsoft/vscode-codicons/blob/main/dist/codicon.ttf) - [*License*](https://github.com/microsoft/vscode-codicons/blob/main/LICENSE), 
[**Ubuntu-Regular.ttf**](https://fonts.google.com/specimen/Ubuntu) - [*License*](https://ubuntu.com/legal/font-licence)

-----
Copyright (c) 2023 MisterRooster ([github.com/MisterRooster](https://github.com/MisterRooster)). All rights reserved.  
Licensed under the MIT license. See [LICENSE](LICENSE) for full terms.
