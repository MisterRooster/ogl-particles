# Known issues

Findings from a static read of the codebase (2026-07-16). Nothing here has been fixed — this is a
record so the next person doesn't rediscover them or "fix" them blind. Every entry was verified
against source; file:line refs are from commit `2707b39`.

Nothing here is urgent for a demo that runs on the author's machine. The ordering is by what would
bite hardest if someone else built or extended the project.

## Crash & correctness risks

### 1. AVX loads are under-aligned

`ParticleData::generate` allocates every array with **16-byte** alignment
(`src/particles/ParticleData.cpp:31-37`), but with `SSE_MODE == SSE_MODE_AVX` the Euler updater
dereferences `__m256*` (`src/particles/ParticleUpdaters.cpp:79-104`), which requires **32**. Aligned
AVX loads on a 16-byte-aligned address fault. It survives today because `_aligned_malloc` frequently
hands back 32-byte-aligned blocks for large allocations, but that is luck, not contract.

`SSE_MODE` is also a hard compile-time `#define` (`ParticleUpdaters.cpp:18`) with no CPUID check, so
the binary crashes on any CPU without AVX. Fixes: bump the alignment to 32, or use
`_mm256_loadu_ps`/`_mm256_storeu_ps`.

### 2. Out-of-bounds write when particle count is odd

`ParticleSystem::update` (`src/particles/ParticleSystem.cpp:50-54`):

```cpp
for (size_t i = 0; i < m_count; i += 2)
{
    m_particles.m_acc[i] = glm::vec4(0.0f);
    m_particles.m_acc[i + 1] = glm::vec4(0.0f);   // i + 1 == m_count when m_count is odd
}
```

With an odd `m_count`, the final iteration writes one `vec4` past the end of the allocation. The
`if (m_count % 2 != 0)` tail block below it (`:55-58`) is then redundant — the loop already wrote
that element (out of bounds). The same pattern appears in the AVX loops, where `i += 2` covers two
`vec4`s per step and can run one past `endId`.

Not hit in practice only because the four shipped effects all use even counts (default 500000).

### 3. `DBG` / `ASSERT` / `FAIL` compile to nothing in Release

`src/utility/Debug.h:24-32` wraps all three macros in `#ifndef NDEBUG`. CMake puts `-DNDEBUG` in the
default Release and RelWithDebInfo flags, so **every error path built on `FAIL` silently vanishes in
a Release build** despite reading like a hard stop. Concretely,
`src/utility/FileSystem.cpp:34-39`:

```cpp
FILE* file = fopen(filepath, "rt");
if (file == NULL)
    FAIL("Unable to open file '%s'\n", filepath);   // no-op in Release

unsigned long size = fileSize(file);                // called with NULL
```

A missing shader or sprite exits cleanly in Debug and crashes in Release. This is the one to fix
first if the app is ever shipped: `FAIL` should stay live in all configs.

### 4. `createLogoTexture` falls off the end of a `bool` function

`UIContext::createLogoTexture` (`src/render/RenderContext.cpp:575-604`) returns `false` on the image
load failure path but has **no `return` on success**. Undefined behavior; MSVC happens to leave a
truthy value in the return register.

### 5. `Shader` constructor passes `nullptr` to `sprintf`

`src/render/Shader.cpp:126` does `sprintf(fullG, "%s%s", path, g)` unconditionally, and both call
sites pass `nullptr` for the geometry stage (`src/ui/SceneView.cpp:64-65`). Passing `nullptr` for
`%s` is UB; MSVC prints `"(null)"` and the string is then never used because of the `if (g)` guard
further down. Guard the `sprintf` instead.

### 6. `Application::instance()` cannot be called

`src/Application.h:34` returns `*_sInstance`, and `_sInstance` (`:43`) is declared but has **no
definition in any translation unit** and is never assigned in the constructor. Any call is a link
error at best. Either wire it up in the ctor and define the static, or delete both.

## Documentation mismatches

### 7. README's OpenGL requirement is wrong

The README claims "OpenGL 4.4 core profile is required", but the context is created as **3.0 with no
core-profile hint** (`src/render/RenderContext.cpp:207-208`) — i.e. compatibility. That is load-bearing,
not cosmetic: `src/shaders/particles.vert:11` reads `gl_Vertex`, a compatibility-only builtin, while
declaring a `vVertex` attribute (`:4`) it never uses. Under a real 4.4 core context the shader would
fail to compile. Requesting core would mean fixing the shader to use `vVertex`.

`common.inc` does say `#version 440`, which is what makes the README's claim look plausible.

### 8. README's dependency versions are stale

Commit `465e8ab` bumped submodules without updating the README:

| README says | Actually pinned |
|---|---|
| glew-cmake 2.2.0 | **glew-cmake 2.3.1** (`2f38fcb`) |
| glm 9.9.8 (`47585fd`) | **glm 1.0.3** (`8d1fd52e`) |

glfw 3.3.8 and imgui v1.89.8-docking do match. Also unmentioned: `dependencies/glfw` and
`dependencies/imgui` point at **personal forks** (`MisterRooster/*`), not upstream.

### 9. README's build command does not run

```bash
cmake -G "Visual Studio 17"..
```

Two defects: no space before `..`, and the generator name is incomplete — CMake wants
`Visual Studio 17 2022`. Working form: `cmake -G "Visual Studio 17 2022" -A x64 ..`

## Portability

### 10. A non-Windows build compiles but is broken

Everything platform-critical sits inside `if (WIN32)` in `CMakeLists.txt`:

- `:47-58` — all `target_compile_definitions`, **including `IMGUI_USER_CONFIG`**. Off Windows, ImGui
  compiles without `src/thirdparty/oglp_imgui_config.h`, so `IMGUI_USE_WCHAR32` and friends silently
  don't apply, and `GLEW_STATIC` is missing despite GLEW being built static.
- `:164-173` — the `data/` and `src/shaders/` copy steps. Off Windows the exe ships with no assets.

So the build succeeds and the app fails at runtime. `${CMAKE_DL_LIBS}` in `APP_LIBS` (`:125`) hints
Unix was once intended. Other blockers if that's ever revived: `_aligned_malloc`/`_aligned_free`
(`ParticleData.cpp`) are MSVC/MinGW-only, and asset paths hardcode backslashes
(`src/ui/SceneView.cpp:52`, `src/render/RenderContext.cpp:231-233`).

### 11. Backslash in an include path

`src/render/BufferObject.h:10` is `#include "GL\glew.h"`. Every other site uses `GL/glew.h`. Only
works on Windows.

## Dead code & cruft

### 12. `src/particles/Solver.*` is unreferenced

`Solver` / `VerletObject` (a 2D Verlet constraint solver) is referenced from nowhere outside itself
and has nothing to do with the vec4/SoA pipeline. The source glob compiles it into every build.

### 13. Redundant work in `ParticleSystem::update`

Accelerations are zeroed **three times** per frame (`src/particles/ParticleSystem.cpp:45-58`): a
scalar loop over all `m_count`, then an unrolled-by-2 loop doing the identical work, then a tail
fixup. The first loop alone is correct and sufficient. In the AVX updater, the
`for (; i < endId; i++)` tail loops (`ParticleUpdaters.cpp:82-85, 94-97, 106-109`) are unreachable —
`i` is already `>= endId` when they're reached.

### 14. Build config that does nothing

- `APP_BUILD_TYPE` is computed (`CMakeLists.txt:9-13`) and never read.
- Warning flags are **commented out** (`:61-67`) — `/W4 /WX` for MSVC, `-Wall -Wformat -Wcast-align`
  otherwise. The project builds at default warning level. Turning them on is how several items above
  would have been caught.
- `file(GLOB_RECURSE ...)` (`:16-27`) has no `CONFIGURE_DEPENDS`, so new source files are ignored
  until someone re-runs `cmake` by hand.
- `target_include_directories` marks three dirs `PUBLIC` on an executable (`:134-136`) — meaningless,
  nothing links it.

### 15. Plumbed but never used

- **Shader hot-reload**: `ShaderObject::refresh()` / `Shader::refresh()` (`src/render/Shader.cpp:24`,
  `:199`) compare file mtimes and recompile, but nothing outside the class ever calls them.
- **Compute & transform-feedback support** in `Shader` (`COMPUTE_SHADER`, `dispatch()`,
  `addFeedbackVarying`) — the particle path is pure vert/frag.
- **Three alternative renderers** (`gl_map`, `gl_double`, `gl_persistent`) are registered in
  `ParticleRendererFactory`, but `main.cpp` hardcodes `"gl"` for all four effects.
- **ImGui demo**: `imgui_demo.cpp` is compiled in (`src/thirdparty/ImGuiBuild.cpp:13`) and
  `ImGui::ShowDemoWindow()` is still called (`src/main.cpp:73`), but
  `IMGUI_DISABLE_DEMO_WINDOWS` (`oglp_imgui_config.h:14`) makes it an empty no-op.
