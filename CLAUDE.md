# CLAUDE.md — learn-qt-2026

## Project Overview

This is a 14-week, project-based curriculum for learning Qt 6 desktop development.

**Constraints**:
- **CMake only** — no qmake, no `.pro` files
- **VS Code + clangd** — no Qt Creator
- **Widgets only** — no QML, no Qt Quick
- **C++17** minimum (Qt 6 requirement)

**Through-line project**: A **Multi-Tab Dev Console** — a desktop application with three tabs:
1. **Log Viewer** — open, parse, filter, and search log files (model/view)
2. **Text Editor** — syntax highlighting, line numbers, find/replace, file browser
3. **Serial Monitor** — serial port communication, hex/ASCII display, threading

The project grows incrementally from Week 5 through Week 14. Source code lives in `project/`.

## Repo Structure

```
learn-qt-2026/
├── README.md           — Repository overview
├── CLAUDE.md           — This file (instructions for Claude)
├── LearningPath.md     — Full topic tree with OPTIONAL markers
├── Schedule.md         — 14-week plan with checkboxes and progress tracking
├── project/            — DevConsole source code (grows incrementally)
│   └── .gitkeep
└── weeks/
    └── week-NN/        — Lesson files for each week
        └── topic-name.md
```

## How to Generate Lessons

1. **Always read `Schedule.md` first** to determine what topics and project milestones to cover for the requested week.
2. Only cover topics listed in the schedule. Respect OPTIONAL markers in `LearningPath.md` — do not include OPTIONAL topics unless explicitly asked.
3. Create lesson files at `weeks/week-NN/topic-name.md` (e.g., `weeks/week-05/file-io.md`).
4. One file per major topic grouping within a week. Use the filenames listed in the schedule's "Lesson files" line.
5. **For Weeks 5–14 (project weeks)**: also generate/update project source code in `project/` matching the week's project milestone.
6. **Update `README.md`** after generating lessons — add or update a `## Lessons` section with links grouped by week. Use the week title from `Schedule.md`:
   ```markdown
   ## Lessons

   ### Week 1 — Introduction to Qt
   - [Introduction to Qt](weeks/week-01/introduction-to-qt.md)
   - [Qt Architecture](weeks/week-01/qt-architecture.md)
   - [Environment Setup](weeks/week-01/environment-setup.md)
   ```
7. **Update `Schedule.md`** — change the week's status from "Not started" to "Done" in the progress tracker.
8. **Add metadata footer** to every lesson file — see the lesson template below for the exact format.

## Lesson File Template

Every lesson file must follow this exact structure:

```markdown
# Topic Name

> One-sentence summary of what this topic is and why it matters.

## Table of Contents
- [Core Concepts](#core-concepts)
- [Code Examples](#code-examples)
- [Common Pitfalls](#common-pitfalls)
- [Key Takeaways](#key-takeaways)
- [Exercises](#exercises)

## Core Concepts

[Explanation broken into logical subsections with ### headings as needed.
For every concept, cover three layers:
1. WHAT — what is this thing?
2. HOW — how does it work / how do you use it?
3. WHY — why does it exist? Why this approach over alternatives? Why does it matter?
The "why" is the most important layer — it builds lasting intuition.
Use `####` subheadings (What / How / Why It Matters) under each `###` concept heading
to keep the three layers visually distinct and scannable.
Do NOT use inline bold labels like "**What.**" — they blend into paragraph text.
Build intuition first, then add precision. Use analogies for abstract ideas.
Keep paragraphs short (3-5 sentences max).

**Mermaid diagrams** — use ```mermaid blocks liberally for:
- Signal-slot connection flows
- Object trees and parent-child relationships
- Model/view architecture diagrams
- Event loop and event propagation flow
- Threading diagrams (main thread ↔ worker thread)
- Class hierarchies
- CMake dependency graphs
Diagrams should appear inline with the concepts they illustrate,
not grouped at the end.]

## Code Examples

[Annotated, idiomatic, production-quality code. Show how a professional
would actually write this — proper naming, const-correctness, clean structure.
Each example should be self-contained and compilable.
Always include necessary #include headers and show both .h and .cpp when relevant.
**CMake only** — always show CMakeLists.txt when demonstrating build setup.
**No Qt Creator** — all examples assume command-line build with cmake + make/ninja.]

## Common Pitfalls

[Bad vs good code comparisons. Each pitfall gets:
- What the mistake is
- Why it's wrong
- The correct approach with code]

## Key Takeaways

- Bullet list of 3-5 most important points
- What to remember, what to internalize

## Exercises

[3–5 exercises per lesson. Mix of:
- Concept questions: "Explain why...", "What happens when...", "What is the difference between..."
- Coding challenges: "Write a program/function that..."
Do not include solutions.]
```

### Project Weeks (5–14): Replacing Exercises with Project Tasks

For lessons in **Weeks 5–14**, replace the `## Exercises` section with `## Project Tasks`:

```markdown
## Project Tasks

[3–5 tasks that advance the DevConsole project. Each task should:
- Reference specific classes/files in `project/`
- Build on the week's concepts
- Contribute to the project milestone described in Schedule.md
- Be concrete: "Add a QAction for File > Open that shows a QFileDialog"
  not vague: "Practice using file dialogs"
Do not include solutions.]
```

Update the Table of Contents link accordingly: `- [Project Tasks](#project-tasks)` instead of `- [Exercises](#exercises)`.

---

## Writing Style

- **Audience**: Self-learner studying independently — no instructor, no classroom.
- **Tone**: Concise, expert, opinionated. Write like a senior engineer mentoring a colleague, not a textbook.
- **Structure**: Build intuition first, then add precision. Use analogies for abstract ideas.
- **Why-first**: For every concept, always explain *why* it exists, *why* this approach, and *why* it matters. The "why" is more important than the "what."
- **Paragraphs**: Keep short — 3-5 sentences max. Dense walls of text kill learning.
- **Mermaid diagrams**: Use them generously. Qt has many systems that are easier to understand visually — object trees, signal-slot wiring, model/view layers, event flow, threading. A well-placed diagram replaces a paragraph of explanation.
- **No external resources**: Do not include "Further Reading" sections or links to external material.

## Code Example Standards

- Write **idiomatic, production-quality Qt code** — the kind a senior engineer would write at work.
- Show professional coding habits: meaningful names, const-correctness, proper error handling, clean structure.
- Every code block must be **self-contained and compilable** (include necessary `#include` headers and `main()` where appropriate).
- Use detailed inline comments to explain *why*, not just *what*.
- When showing a pattern or technique, show the **realistic use case**, not a toy example.
- Show both header (`.h`) and implementation (`.cpp`) when demonstrating class definitions.
- **Build system**: Always CMake. Show `CMakeLists.txt` when relevant. Never use qmake or `.pro` files.
- **Editor**: All instructions assume VS Code + clangd. No Qt Creator references.

## Common Pitfalls Format

Each pitfall must include:

1. A brief description of the mistake
2. A `// BAD` code block showing the wrong way
3. An explanation of *why* it's wrong
4. A `// GOOD` code block showing the correct approach

```cpp
// BAD — [description of what's wrong]
[bad code]

// GOOD — [description of the fix]
[good code]
```

---

## `project/` Directory Conventions

The `project/` directory contains the DevConsole source code. It grows incrementally:

- **Week 5**: Initial skeleton — `CMakeLists.txt`, `main.cpp`, `MainWindow.h/.cpp`
- **Each subsequent week**: New files added for that week's feature (e.g., `LogViewer.h/.cpp`, `SerialMonitor.h/.cpp`)
- **Build system**: Single top-level `CMakeLists.txt` in `project/`
- **Structure**: Flat initially, organize into subdirectories only when complexity warrants it
- **Every file**: Include clear header comments stating which week introduced it

The project must always build and run after each week's additions. No broken intermediate states.

---

## Git Workflow

After generating lessons and project code for a week:

1. Update `README.md` (add lesson links under `## Lessons`)
2. Update `Schedule.md` (mark the week as "Done" in the progress tracker)
3. Use `/commit` to review and commit all changes together
4. **Commit message format**: `week-NN: <week title>` (e.g., `week-05: file-io and dialogs`)
5. Commit lessons (`weeks/week-NN/`), project code (`project/`), and updated `README.md` + `Schedule.md` together in one commit per week

---

## Repo-Specific: Qt 6 Widgets Desktop Development

### Language & Framework

- **Language**: C++ with Qt 6 framework
- **Target standard**: **C++17** (Qt 6 requires C++17 minimum)
- **Qt version**: **Qt 6.x** — do not use deprecated Qt 5 APIs
- **Build system**: **CMake only** — no qmake, no `.pro` files
- **Editor**: **VS Code + clangd** — no Qt Creator
- **UI framework**: **Qt Widgets only** — no QML, no Qt Quick
- **Compiler assumption**: clang++ or g++ with `-std=c++17`
- **Focus area**: Developer tools — serial monitors, log viewers, text editors

### Qt Coding Conventions

- Use the **new signal-slot syntax** (function pointers) over the old `SIGNAL()`/`SLOT()` macros
- Use `QString` for text, not `std::string` — Qt APIs expect `QString`
- Use Qt containers (`QList`, `QMap`) in Qt API boundaries; `std::vector`, `std::map` for internal logic where Qt APIs aren't involved
- Use `Q_OBJECT` macro in every QObject subclass that defines signals or slots
- Follow Qt naming: `camelCase` for functions/variables, `PascalCase` for classes, `m_` prefix for member variables (optional but common)

### Memory Management in Qt

- Qt's **parent-child ownership** model: parent deletes its children — do not double-delete
- Use raw `new` for QObjects with a parent (parent takes ownership) — this is idiomatic Qt
- Use `std::unique_ptr` for QObjects **without** a parent
- Never use `std::shared_ptr` for QObjects — it conflicts with Qt's ownership model
- Understand that `deleteLater()` schedules deletion in the event loop — use it for objects involved in signal-slot connections

### Qt-Specific Teaching Notes

- Always explain the **MOC (Meta-Object Compiler)** implications — what it generates, why it's needed
- When discussing signals/slots, explain the difference between **direct** and **queued** connections and when each is used
- Use **Mermaid diagrams** for: object trees, signal-slot connections, model/view architecture, event loop flow, threading, class hierarchies, CMake dependency graphs
- Compare Qt approaches with standard C++ alternatives where relevant (e.g., Qt signals vs std::function callbacks)
- **No QML** — do not reference QML, Qt Quick, or QML engine ownership rules
- **No Qt Creator** — all workflows use command-line CMake builds and VS Code
