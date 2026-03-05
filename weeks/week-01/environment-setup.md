# Environment Setup

> Setting up Qt 6 with CMake and VS Code gives you a professional, lightweight development environment — no IDE lock-in, full control over the build process, and excellent code intelligence via clangd.

## Table of Contents

- [Core Concepts](#core-concepts)
  - [Installing Qt 6](#installing-qt-6)
  - [CMake with Qt 6](#cmake-with-qt-6)
  - [VS Code + clangd Setup](#vs-code--clangd-setup)
  - [Building and Running from Terminal](#building-and-running-from-terminal)
- [Code Examples](#code-examples)
- [Common Pitfalls](#common-pitfalls)
- [Key Takeaways](#key-takeaways)
- [Exercises](#exercises)

---

## Core Concepts

### Installing Qt 6

#### What

Qt 6 can be installed via the online installer (all platforms) or Homebrew (macOS). You need the Qt libraries, headers, and CMake config files. The install method determines where these files land on disk, and that path is what CMake needs to find everything.

#### How

**macOS (Homebrew — preferred)**

```bash
# Install Xcode command line tools first
xcode-select --install

# Install Qt 6
brew install qt@6

# Homebrew installs Qt as "keg-only" — add it to your PATH
# Apple Silicon:
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
# Intel Mac:
export PATH="/usr/local/opt/qt@6/bin:$PATH"

# Verify — check that Qt binaries are on PATH
qt-cmake --version
```

Homebrew installs Qt to `/opt/homebrew/opt/qt@6` on Apple Silicon or `/usr/local/opt/qt@6` on Intel. This is a "keg-only" install, meaning Homebrew doesn't symlink it into `/usr/local/bin` to avoid conflicts. You must tell CMake where to find it.

**Windows (Qt Online Installer)**

1. Download the installer from [qt.io](https://www.qt.io/download-qt-installer-oss).
2. Sign in (free account required) and select Qt 6.7+ with the MSVC kit (recommended) or MinGW kit.
3. Default install path: `C:\Qt\6.7.0\msvc2019_64` or similar.
4. Set the prefix path so CMake can find it:

```cmd
set CMAKE_PREFIX_PATH=C:\Qt\6.7.0\msvc2019_64
```

Also install CMake and Ninja if not already available:

```cmd
winget install Kitware.CMake
pip install ninja
```

#### Why It Matters

Getting the install right on the first try saves hours of debugging. The most common beginner issue is CMake not finding Qt because `CMAKE_PREFIX_PATH` isn't set. On macOS, Homebrew's keg-only behavior means Qt isn't on the default search path. On Windows, the Qt installer puts files in a custom directory that CMake doesn't know about.

---

### CMake with Qt 6

#### What

CMake is the only officially supported build system for Qt 6. You use `find_package` to locate Qt, `qt_add_executable` to create targets with MOC support, and `target_link_libraries` to link modules. This is the standard Qt 6 build workflow.

#### How

Here is a complete, minimal `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my-qt-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(my-qt-app
    main.cpp
)

target_link_libraries(my-qt-app PRIVATE Qt6::Widgets)
```

Line-by-line breakdown:

| Line | Purpose |
|------|---------|
| `cmake_minimum_required(VERSION 3.16)` | Qt 6 requires CMake 3.16+. This enforces it. |
| `project(my-qt-app LANGUAGES CXX)` | Declares the project name and that we use C++. |
| `set(CMAKE_CXX_STANDARD 17)` | Qt 6 requires C++17 minimum. |
| `set(CMAKE_CXX_STANDARD_REQUIRED ON)` | Fail the build if C++17 isn't available — don't silently fall back. |
| `find_package(Qt6 REQUIRED COMPONENTS Widgets)` | Locates Qt 6 on disk. `REQUIRED` makes it a fatal error if not found. |
| `qt_add_executable(my-qt-app main.cpp)` | Creates the executable with AUTOMOC, AUTOUIC, AUTORCC enabled. |
| `target_link_libraries(... PRIVATE Qt6::Widgets)` | Links the Widgets module. `PRIVATE` means dependents don't inherit it. |

The key insight is `qt_add_executable` vs plain `add_executable`. The Qt version automatically enables the Meta-Object Compiler (MOC), which processes classes containing `Q_OBJECT`. Without it, you'd need to configure AUTOMOC manually.

```mermaid
flowchart LR
    A[CMakeLists.txt] --> B[cmake -B build]
    B --> C[find_package finds Qt6]
    C --> D[AUTOMOC scans for Q_OBJECT]
    D --> E[cmake --build build]
    E --> F[MOC + Compiler + Linker]
    F --> G[Executable]
```

#### Why It Matters

`qt_add_executable` handles AUTOMOC, AUTOUIC, and AUTORCC automatically — without it, you'd need to configure these manually, and forgetting to do so causes cryptic linker errors. Understanding CMake is essential because every Qt 6 project uses it, and it's the foundation for adding modules, resources, and tests later.

---

### VS Code + clangd Setup

#### What

VS Code with clangd provides IDE-quality features: autocomplete, go-to-definition, inline errors, refactoring — all powered by the same compiler frontend (clang) that builds your code. It replaces the default C/C++ IntelliSense extension with something that actually understands your build system.

#### How

**Step 1 — Install extensions**

Install two VS Code extensions:
- `clangd` by LLVM — provides code intelligence
- `CMake Tools` by Microsoft — provides CMake integration

Disable the Microsoft C/C++ extension's IntelliSense if installed, as it conflicts with clangd.

**Step 2 — Generate `compile_commands.json`**

```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

This creates `build/compile_commands.json`, a file that tells clangd exactly how each source file is compiled — including all Qt include paths, defines, and flags.

**Step 3 — Create `.clangd` config**

Create a `.clangd` file in your project root:

```yaml
CompileFlags:
  CompilationDatabase: build
Diagnostics:
  UnusedIncludes: Strict
```

**Step 4 — Configure VS Code workspace settings**

Create `.vscode/settings.json`:

```json
{
    "clangd.arguments": [
        "--compile-commands-dir=build",
        "--header-insertion=never"
    ],
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja",
    "editor.formatOnSave": true,
    "[cpp]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
    }
}
```

After this, restart VS Code. Open a `.cpp` file and verify that hovering over `QWidget` shows documentation and that autocomplete suggests Qt types.

#### Why It Matters

Without clangd, VS Code is just a text editor. With it, you get the same code intelligence as a full IDE. The key is `compile_commands.json` — it tells clangd exactly how your code is compiled, including Qt include paths. If autocomplete doesn't work, the problem is almost always a missing or stale `compile_commands.json`.

---

### Building and Running from Terminal

#### What

Building Qt apps from the command line using `cmake` and Ninja. This is the workflow you'll use daily: configure once, build repeatedly, run the output binary.

#### How

**Basic workflow:**

```bash
# Configure (one time, or when CMakeLists.txt changes)
cmake -B build -G Ninja

# Build
cmake --build build

# Run
./build/my-qt-app        # macOS/Linux
.\build\my-qt-app.exe    # Windows
```

On Windows with MSVC, you may need to run from "Developer Command Prompt for VS" or call `vcvarsall.bat` first to set up compiler environment variables.

**Using CMake Presets (recommended for cross-platform):**

Create a `CMakePresets.json` in your project root:

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "default",
            "binaryDir": "${sourceDir}/build",
            "generator": "Ninja",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "default",
            "configurePreset": "default"
        }
    ]
}
```

With presets, the workflow simplifies to:

```bash
cmake --preset default
cmake --build --preset default
./build/my-qt-app
```

Presets eliminate the need to remember flags. They're version-controlled, so every developer on the team gets the same configuration.

#### Why It Matters

Command-line builds are reproducible, scriptable, and work in CI/CD. No IDE dependency. Ninja is preferred over Make because it's significantly faster for incremental builds — it tracks file dependencies more precisely and parallelizes better. On a project with 100+ source files, the difference is noticeable.

---

## Code Examples

### Example 1 — Complete Cross-Platform CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(qt-starter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(qt-starter
    main.cpp
)

target_link_libraries(qt-starter PRIVATE Qt6::Widgets)
```

### Example 2 — Minimal QApplication + QWidget App

```cpp
// main.cpp — minimal Qt Widgets window
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Qt Starter");
    window.resize(400, 300);
    window.show();

    return app.exec();
}
```

### Example 3 — VS Code Workspace Settings

`.vscode/settings.json`:

```json
{
    "clangd.arguments": [
        "--compile-commands-dir=build",
        "--header-insertion=never"
    ],
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja",
    "editor.formatOnSave": true,
    "[cpp]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
    }
}
```

### Example 4 — `.clangd` Config File

```yaml
CompileFlags:
  CompilationDatabase: build
Diagnostics:
  UnusedIncludes: Strict
```

### Example 5 — CMakePresets.json for Cross-Platform

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "default",
            "binaryDir": "${sourceDir}/build",
            "generator": "Ninja",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "default",
            "configurePreset": "default"
        }
    ]
}
```

### Example 6 — Full Terminal Build Workflow

```bash
# macOS — install Qt via Homebrew
brew install qt@6 cmake ninja

# Windows — after Qt installer, set prefix path
# set CMAKE_PREFIX_PATH=C:\Qt\6.7.0\msvc2019_64

# Configure
cmake --preset default
# or: cmake -B build -G Ninja

# Build
cmake --build build

# Run
./build/qt-starter
```

---

## Common Pitfalls

### 1. Missing CMAKE_PREFIX_PATH

```bash
# BAD — CMake can't find Qt
cmake -B build
# Error: Could not find a package configuration file provided by "Qt6"

# GOOD — tell CMake where Qt is installed
# macOS (Homebrew, Apple Silicon):
cmake -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6

# Windows:
cmake -B build -DCMAKE_PREFIX_PATH=C:/Qt/6.7.0/msvc2019_64
```

**Why:** CMake doesn't search arbitrary directories. Homebrew installs Qt in a keg-only location that isn't on the default search path. On Windows, the Qt installer puts Qt in a custom directory. `CMAKE_PREFIX_PATH` tells CMake where to look.

### 2. Not Generating compile_commands.json

```cmake
# BAD — clangd has no compile information, shows errors everywhere
cmake -B build

# GOOD — export compilation database for clangd
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# Or add to CMakeLists.txt:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

**Why:** clangd needs `compile_commands.json` to understand your project's include paths and compiler flags. Without it, clangd can't find Qt headers and reports false errors on every `#include <QWidget>`.

### 3. Wrong Generator on Windows

```bash
# BAD — using default Visual Studio generator (slow, cluttered)
cmake -B build
# Generates .sln and .vcxproj files, slow incremental builds

# GOOD — use Ninja for fast, clean builds
cmake -B build -G Ninja
# Install Ninja via: pip install ninja / choco install ninja
```

**Why:** The default CMake generator on Windows is "Visual Studio", which creates IDE project files and builds slower. Ninja generates simple build files and has much faster incremental builds. It also produces a flat `build/` directory instead of nested configuration folders.

---

## Key Takeaways

- Install Qt 6 via Homebrew (macOS) or online installer (Windows) — then set `CMAKE_PREFIX_PATH` so CMake can find it.
- Always set `CMAKE_PREFIX_PATH` if CMake reports `Could not find a package configuration file provided by "Qt6"`.
- Use `qt_add_executable` instead of `add_executable` for automatic MOC support — this is required for any class with `Q_OBJECT`.
- Generate `compile_commands.json` with `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to give clangd full code intelligence.
- Use Ninja as the CMake generator for faster builds: `cmake -B build -G Ninja`.

---

## Exercises

1. **Verify your Qt installation.** Install Qt 6 on your system and verify the installation by checking that `find_package(Qt6)` succeeds in a test `CMakeLists.txt`. Create a minimal CMakeLists.txt with `find_package(Qt6 REQUIRED COMPONENTS Widgets)` and run `cmake -B build` to confirm Qt is found.

2. **Build the starter project from scratch.** Create the "qt-starter" project: write `CMakeLists.txt` and `main.cpp` from the code examples above, build it from the terminal with Ninja, and run it. Verify that a 400x300 window titled "Qt Starter" appears.

3. **Set up VS Code with clangd.** Configure your project with the `.clangd` file and `.vscode/settings.json` from the examples. Open `main.cpp` in VS Code and verify that autocomplete works for Qt types — type `QWi` and confirm `QWidget` appears in suggestions. If it doesn't, check that `build/compile_commands.json` exists and is not empty.

4. **Add CMake presets.** Modify the starter project to use `CMakePresets.json`. Add a `"release"` preset that sets `CMAKE_BUILD_TYPE` to `Release`. Build with both presets and compare the executable sizes. The release build should be noticeably smaller due to optimizations and stripped debug info.

---
up:: [Schedule](../../Schedule.md)
#type/learning #source/self-study #status/seed
