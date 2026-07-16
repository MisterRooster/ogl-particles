# ogl-particles

CPU-solved (SSE/AVX), GPU-rendered particle system with a Dear ImGui interface. C++17, CMake, single
executable target. Windows-only in practice: all preprocessor defines and the asset-copy steps live
inside `if (WIN32)` in `CMakeLists.txt`.

Dependencies are git submodules under `dependencies/` (glfw, glm, imgui, glew-cmake) and auto-init at
configure time — no manual `git submodule update` needed. ImGui is not a CMake subproject; it is
compiled into the exe by the unity file `src/thirdparty/ImGuiBuild.cpp`.

`docs/known-issues.md` records pre-existing bugs, dead code, and doc mismatches. Check it before
"fixing" something that looks wrong — it is probably already logged and deliberately unfixed.

## Build & run

```sh
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 ..     # tested path -> bin/Release/ogl-particles.exe
cmake --build . --config Release
```

The existing `build/` tree here is configured for MinGW instead: `cmake -G "MinGW Makefiles"
-DCMAKE_BUILD_TYPE=Debug ..`, which produces `bin/ogl-particles.exe` (no per-config subdir).

- Sources are `GLOB_RECURSE`d without `CONFIGURE_DEPENDS`. A new file needs **no CMake edit**, but you
  must re-run `cmake` before it compiles.
- `data/` and `src/shaders/` are copied next to the exe post-build. Assets resolve relative to the
  **exe**, not the cwd (`FileSystem::getModuleDirectory()`), so run it from where it was built.
- Debug builds get a console; Release links `/SUBSYSTEM:WINDOWS`.
- There are no tests, no CI, and no formatter config. Verification means building and running the app.

## Layout

All of `src/` is one flat `namespace nhahn` — directories do **not** map to sub-namespaces.

| Directory | Contents |
|---|---|
| `src/particles/` | `ParticleSystem`, SoA `ParticleData`, generators/updaters, renderers, the 4 effects |
| `src/render/` | `GLContext`/`UIContext`, `Shader`, `Texture`, `RenderTarget`, buffer wrappers |
| `src/ui/` | `Window`, `SceneView` (viewport + camera), `PropertyPanel`, ImGui widgets |
| `src/input/` | `Input` singleton (reach it via `gInput()`), XInput `Gamepad` |
| `src/utility/` | `Debug` (logging), `FileSystem`, `Timer`, `PRNG.h`, `Utils`, `Types.h` |
| `src/shaders/` | GLSL, loaded from disk at runtime; `common.inc` is prepended as source #0 |
| `src/thirdparty/` | imgui unity build, imgui config override, stb_image |

## Conventions

No `.clang-format` exists — these are held by hand. Match the file you are editing.

**Every file** opens with this banner, byte-for-byte, with **no blank line after it**. Line 8 is
`#pragma once` (headers) or `#include "OwnHeader.h"` (sources). Two blank lines before `namespace nhahn`.

```
/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
```

- **Members are `_camelCase` — except in `src/particles/`, which uses `m_camelCase`.** That module was
  adapted from Filipek's particle system and kept its prefix. This is the easiest thing to get wrong.
- Types `PascalCase`; pure interfaces take an `I` prefix (`IEffect`, `IParticleRenderer`). Methods
  `camelCase`, getters usually drop `get` (`width()`, `numAliveParticles()`). A leading `_` on a
  *public* method marks framework-glue-only (`_onResize`, `_notifyKeyChanged`).
- Tabs, width 4 (13 files use spaces), Allman braces, ~100 columns, CRLF. Pointers bind left: `int* p`.
  Access specifiers sit at the same indent as `class`; private methods and private data get separate
  `private:` sections.
- `#pragma once` always, never include guards. Project includes use quotes rooted at `src/`
  (`#include "utility/Debug.h"`); siblings use a bare filename; std uses `<>`.
- `/** One-line summary. */` above classes. Methods are usually left undocumented.
- `std::shared_ptr`/`make_shared` for ownership, raw pointers only for non-owning observers. `using`
  aliases, never `typedef`. `explicit` single-arg ctors and `= delete`d copies on the heavy types.
- Unscoped `enum`s with a type-abbreviation prefix on the values (`IK_SPACE`, `TEXEL_FLOAT`), because
  the namespace is flat.

**Logging — no exceptions are thrown anywhere in this codebase.** From `src/utility/Debug.h`:

```cpp
DBG("FountainEffect", DebugLevel::DEBUG, "Particles memory usage: %dmb\n", mb);
ASSERT(sys != nullptr, "GLParticleRenderer: particle system is null");
FAIL("Unable to open file '%s'\n", filepath);
```

The tag is the class/module name, and the format string **must end with `\n`** — nothing appends one.
`ASSERT`/`FAIL` print file:line and `exit(EXIT_FAILURE)`.

## Adding a particle effect

There is no self-registration; a new effect touches four places.

1. `src/particles/XEffect.{h,cpp}` implementing all of `IEffect` (`Effect.h`). Copy `FountainEffect`
   as the template. Hold as members **only** the generators/updaters you expose in `renderUI()`.
2. In `initialize()`: build the `ParticleSystem`, then a `ParticleEmitter` with one generator per
   attribute — position, color, velocity, and `BasicTimeGen` (**without it nothing ever dies**) —
   then add updaters. Order matters: `BasicTimeUpdater` → a color updater → `EulerUpdater` →
   collision/`FloorUpdater` last.
3. Add an `else if (effect == "x")` branch to `EffectFactory::create` in `Effect.cpp`.
4. In `main.cpp`: add the global, the `create`/`initialize`/`initializeRenderer` triple, the
   `propertyPanel->addEffect(...)`, and the `clean()` call.

New *behavior* rather than a new arrangement belongs in `ParticleGenerators.*` (fill `[startId, endId)`)
or `ParticleUpdaters.*` (walk `[0, m_countAlive)`). Across updaters, `m_time` means: `.x` remaining
life, `.y` max life, `.z` normalized age 0→1, `.w` `1/maxLife`.

## Invariants to preserve

- `ParticleData` is **SoA** (parallel `glm::vec4*` arrays), and alive particles are kept **packed at
  the front**: `wake`/`kill` swap with the boundary element and move `m_countAlive`. This is why the
  renderer can upload `[0, m_countAlive)` as one contiguous range.
- Particles are stepped inside `SceneView::render`, not the app loop: `update` (animate params) →
  `cpuUpdate` (solve) → `gpuUpdate` (VBO upload) → `render` (`glDrawArrays(GL_POINTS, ...)`).
- `main.cpp`'s `render()` runs **inside an already-open ImGui frame** — `Window::preRender`/
  `postRender` bracket it, so call ImGui directly and never start a frame yourself.
