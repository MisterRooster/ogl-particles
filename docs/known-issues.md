# Known issues

Findings from a static read of the codebase, recorded 2026-07-16 and worked through the same day.
Everything in the first section has since been fixed; it is kept as a short record of what was wrong
and why, since several of these explain odd-looking code. The open items at the bottom are the ones
still live.

## Open

### Release builds report failures silently

`ASSERT` and `FAIL` now run in every configuration and exit with `EXIT_FAILURE`, but they report
through `printf`. Release links `/SUBSYSTEM:WINDOWS` (`-mwindows` under GCC), so there is no console
attached and the process just disappears from the user's point of view. The exit code is still
correct, and a debugger or a shell that checks `%ERRORLEVEL%` sees it. Surfacing a `MessageBox` on
Windows would close the gap.

### Non-Windows builds are still not possible

The build settings and asset paths are no longer Windows-gated, but the code itself is:
`RenderContext.cpp` has a Win32 `WindowProc`, `disableTitlebarWin32`, and `dwmapi` calls for the
custom titlebar, and `Gamepad.cpp` is XInput. A Linux or macOS port needs those abstracted or
stubbed; nothing else is known to block it.

### Plumbed but never used

Not bugs, just weight to be aware of before assuming a feature works:

- **Shader hot-reload**: `ShaderObject::refresh()` / `Shader::refresh()` (`src/render/Shader.cpp:24`,
  `:199`) compare file mtimes and recompile, but nothing outside the class calls them.
- **Compute and transform-feedback support** in `Shader` (`COMPUTE_SHADER`, `dispatch()`,
  `addFeedbackVarying`) — the particle path is pure vert/frag.
- **Three alternative renderers** (`gl_map`, `gl_double`, `gl_persistent`) are registered in
  `ParticleRendererFactory`, but `main.cpp` hardcodes `"gl"` for all four effects.

### The GL context is 3.0 compatibility, not 4.4 core

Worth knowing before touching the render path. The context is created as 3.0 with no core-profile
hint (`src/render/RenderContext.cpp:207-208`) while the shaders declare `#version 440`; drivers
accept the mismatch. The draw path depends on compatibility-only entry points (`glTexEnvi`,
`GL_POINT_SPRITE` in `src/ui/SceneView.cpp`), so moving to a core profile is a real port, not a
context-hint change. The README now states the actual requirement.

## Fixed

Each of these has a commit with the details.

| Was | Effect |
|---|---|
| `createLogoTexture` fell off the end of a `bool` function | GCC compiled the fall-through to `ud2`; the app died at startup with an illegal instruction |
| `particles.vert` used `gl_Vertex` under `#version 440` | Shader failed to compile, `FAIL` killed the app before the first frame |
| `AttractorUpdater` used MSVC-only `__m128::m128_f32`, and AVX had no arch flag | GCC could not compile the project at all |
| `ASSERT`/`FAIL` compiled out under `NDEBUG` | Release turned clean exits into crashes; `readTextFile` fell through to `fileSize(NULL)` |
| `Utils::sfrand` punned an `int` through a `float*` | Broke strict aliasing; optimised builds could return uninitialised garbage to every particle generator |
| `ParticleData` allocated 16-byte aligned; AVX needs 32 | Aligned AVX loads faulted unless the allocator over-aligned by luck |
| AVX loops tested `i < endId` while stepping 2 | Read/wrote one element past the last particle on odd counts; scalar tails were unreachable |
| `ParticleSystem::update` zeroed `m_acc` three times | The unrolled loop wrote one `vec4` past the allocation on odd counts |
| `delete[]` on a `void*` from stb_image | Mismatched allocator, latent heap corruption |
| `M_PI` defined unguarded in `ParticleSystem.h` | Redefined the system macro and silently overrode the more accurate guarded definitions elsewhere |
| No CPU check for AVX | Illegal instruction with no explanation on non-AVX CPUs; now reported at startup |
| `Shader` ctor passed `nullptr` to `sprintf("%s%s")` | UB on every construction; both call sites pass null for the geometry stage |
| `Application::instance()` dereferenced an undefined static | Would not link if called |
| Asset copies were `POST_BUILD` on the exe | Editing a shader alone left a stale copy deployed and the app silently loaded it |
| Warning flags were commented out | Hid most of the defects above |
| `Solver.*`, `inverse()`/`inverse2()`, `imgui_demo.cpp`, `APP_BUILD_TYPE` | Dead code compiled into every build |
| README: wrong build command, stale dep versions, wrong GL requirement | Documentation did not match the project |
