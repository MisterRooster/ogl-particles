# How to compile with VS Code

## Prerequisites

To be able to compile C++ projects using CMake from within VS Code you need the following requirements:
 - You'll need to have [Cmake](https://cmake.org/), a compiler, a debugger and build tools installed
 - [C++ extension for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) : install by searching for 'c++' in the **Extension** view (*CTRL+SHIFT+X*)
  - [CMake Tools extension for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) : install by searching for 'Cmake tools' in the **Extension** view (*CTRL+SHIFT+X*)

The following guides will help you with setting up and using C++ and CMake Tools for VS Code:
 - [C/C++ for Visual Studio Code](https://code.visualstudio.com/docs/languages/cpp)
 - [CMake Tools for Visual Studio Code documentation](https://github.com/microsoft/vscode-cmake-tools/tree/main/docs#cmake-tools-for-visual-studio-code-documentation)

## Compile the project

### 1. Clone the repository

```bash
git clone git://github.com/MisterRooster/ogl-particles.git
```

### 2. Open the Project in VS Code

```bash
cd ogl-particles
code .
```

### 3. Configure CMake Tools extension

 - Open the Command Palette (*CTRL+SHIFT+P*) and run **CMAKE: Select a Kit** command to select a compiler toolchain.
 - Open the Command Palette (*CTRL+SHIFT+P*) and run **CMAKE: Select Variant** command to select a build type.

### 4. Generate build files

 - Open the Command Palette (*CTRL+SHIFT+P*) and run **CMake: Configure** command to configure your project and generates build files in the project's *build* folder.

### 5. Build project

- Open the Command Palette (*CTRL+SHIFT+P*) and run **CMAKE: Build** command or press the **Build** button from the status bar.

The compiled executables can be found in the project *bin/BuildType* folders.

-----
Copyright (c) 2022 MisterRooster ([github.com/MisterRooster](https://github.com/MisterRooster)). All rights reserved.  
Licensed under the MIT license. See [LICENSE](LICENSE) for full terms.