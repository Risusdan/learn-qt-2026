# Qt 6 Learning Schedule

**Scope**: 14-week curriculum — 4 weeks of fundamentals + 10 weeks building a Multi-Tab Dev Console.
All required topics from the [Learning Path](LearningPath.md). OPTIONAL topics excluded unless explicitly requested.

## Progress Tracker

| Week | Topic | Status |
| ---- | ----- | ------ |
| 1 | Introduction to Qt | Done |
| 2 | Qt Object Model & Type System | Done |
| 3 | Signals & Slots | Done |
| 4 | Widgets, Layouts & Main Window | Done |
| 5 | File I/O & Dialogs | Done |
| 6 | Log Viewer — Basic | Done |
| 7 | Model/View Programming | Done |
| 8 | Filtering, Delegates & Search | Not started |
| 9 | Text Editor — Core | Not started |
| 10 | Text Editor — Features | Not started |
| 11 | Serial Monitor — Connection | Not started |
| 12 | Threading | Not started |
| 13 | Serial Monitor — Advanced | Not started |
| 14 | Cross-Platform & Deployment | Not started |

---

## Phase 1: Qt Fundamentals (Weeks 1–4)

### Week 1: Introduction to Qt

- [ ] History of Qt — Trolltech origins, open-source evolution, The Qt Company today
- [ ] Why Qt? — cross-platform single codebase, mature ecosystem, C++ performance
- [ ] Qt vs alternatives — GTK, wxWidgets, Dear ImGui, native frameworks
- [ ] Qt 6 module overview — QtCore, QtWidgets, QtNetwork, QtSerialPort
- [ ] Meta-Object System — MOC overview, what it generates, why Qt needs it
- [ ] CMake with Qt 6 — `find_package(Qt6)`, `qt_add_executable`, target linking
- [ ] VS Code + clangd setup — compile_commands.json, extensions, debugging
- [ ] First app — minimal QApplication + QWidget, build and run from terminal

**Lesson files**: `introduction-to-qt.md`, `qt-architecture.md`, `environment-setup.md`

**Why It Matters**: Qt is one of the most mature cross-platform frameworks available. Understanding its architecture — especially the meta-object system (MOC) that powers signals/slots, properties, and introspection — is essential for writing idiomatic Qt code. Setting up CMake + VS Code + clangd from day one establishes a professional toolchain that scales to real projects.

---

### Week 2: Qt Object Model & Type System

- [ ] QObject — the root of Qt's object system, what it enables
- [ ] Parent-child ownership — automatic memory management, object trees
- [ ] `Q_OBJECT` macro — enabling signals, slots, properties, introspection
- [ ] QString — implicit sharing, unicode, conversions to/from std::string
- [ ] QList — the default container in Qt 6 (formerly QVector)
- [ ] QVariant — type-erased value container, when to use it
- [ ] Implicit sharing (COW) — how Qt avoids unnecessary copies
- [ ] QTest framework — `QCOMPARE`, `QVERIFY`, data-driven tests, `QSignalSpy`
- [ ] qDebug / qWarning / qCritical — Qt logging functions
- [ ] QLoggingCategory — structured, filterable logging

**Lesson files**: `qt-object-model.md`, `qt-types.md`, `testing-and-debugging.md`

**Why It Matters**: QObject's parent-child model is Qt's answer to memory management — when a parent is destroyed, all its children go with it. This is RAII applied to object graphs. Getting comfortable with Qt's type system (especially implicit sharing) prevents performance surprises. Introducing QTest in Week 2 means you can write tests for everything that follows — testing isn't an afterthought.

---

### Week 3: Signals & Slots

- [ ] Signal-slot mechanism — Qt's observer pattern, MOC-generated glue code
- [ ] Old vs new connection syntax — string-based vs function pointer, always use new
- [ ] Custom signals and slots — declaring in your own QObject subclasses
- [ ] Queued vs direct connections — thread-safe communication via the event loop
- [ ] Lambda slots — inline lambdas, capture pitfalls
- [ ] Event loop — `QApplication::exec()`, event dispatching
- [ ] Event handling — reimplementing event handlers (`mousePressEvent`, etc.)
- [ ] Event filters — intercepting events before they reach the target

**Lesson files**: `signals-and-slots.md`, `event-loop.md`

**Why It Matters**: Signals and slots are the heart of Qt. They replace raw callbacks and manual observer patterns with a type-safe, decoupled communication mechanism. The new connection syntax catches errors at compile time. Understanding the event loop is equally fundamental — it's the engine that drives the entire application, from GUI rendering to signal delivery to timer callbacks.

---

### Week 4: Widgets, Layouts & Main Window

- [ ] QWidget — the base class for all visible UI elements
- [ ] Layouts — QHBoxLayout, QVBoxLayout, QGridLayout, QFormLayout
- [ ] Common widgets — QPushButton, QLabel, QLineEdit, QComboBox, QCheckBox
- [ ] Size policies — how widgets negotiate space within layouts
- [ ] QMainWindow — central widget, dock areas, status bar
- [ ] Menus and toolbars — QMenuBar, QToolBar, QAction
- [ ] Keyboard shortcuts — QKeySequence, standard shortcuts
- [ ] Status bar — QStatusBar, temporary and permanent messages

**Lesson files**: `widgets-and-layouts.md`, `main-window.md`

**Why It Matters**: Everything visible in a Qt Widgets application is a QWidget. Understanding layouts (not absolute positioning) is what makes your UIs resize correctly across screen sizes and platforms. QMainWindow gives you the standard desktop application shell — menus, toolbars, status bar — that users expect. This is the last purely conceptual week; from Week 5, everything you learn goes directly into the DevConsole.

---

## Phase 2: Building the DevConsole (Weeks 5–14)

### Week 5: File I/O & Dialogs

- [ ] QFile and QTextStream — reading/writing text files
- [ ] QSaveFile — atomic writes for safe saving
- [ ] QSettings — persistent application settings
- [ ] QStandardPaths — platform-correct paths for config/data/cache
- [ ] QFileDialog — open/save dialogs
- [ ] QMessageBox — info, warning, question dialogs
- [ ] Custom dialogs — subclassing QDialog, accept/reject flow

**Lesson files**: `file-io.md`, `dialogs.md`

**Project Milestone**: DevConsole shell — QMainWindow with QTabWidget (empty tabs for Log Viewer, Text Editor, Serial Monitor), file open/save dialogs, QSettings for window geometry and recent files.

**Why It Matters**: Every real desktop application reads and writes files and shows dialogs. QSettings gives you persistent configuration for free. This week produces the skeleton of the DevConsole that all subsequent weeks will build on — a proper QMainWindow with a tab container, ready for content.

---

### Week 6: Log Viewer — Basic

- [ ] QPlainTextEdit — efficient plain text display
- [ ] QTextDocument — block structure, append-only patterns
- [ ] Line limiting — preventing unbounded memory growth
- [ ] QMimeData — packaging data for drag/drop
- [ ] Drag events — `dragEnterEvent`, `dropEvent`, accepting specific MIME types

**Lesson files**: `text-display.md`, `drag-and-drop.md`

**Project Milestone**: Log Viewer tab — open and display log files in a QPlainTextEdit, drag & drop log files onto the viewer, auto-scroll toggle, line count display in status bar.

**Why It Matters**: A log viewer is the simplest useful tool in the DevConsole. QPlainTextEdit handles large text efficiently (unlike QTextEdit). Drag and drop is the expected UX for a desktop tool — users want to drop a file and see it. This week gives you a working, useful tab.

---

### Week 7: Model/View Programming

- [ ] Model/View architecture — separating data from presentation
- [ ] QAbstractTableModel — custom model with roles, indexes, headers
- [ ] QTableView — displaying tabular data, column sizing, selection
- [ ] Data roles — DisplayRole, ForegroundRole, ToolTipRole, custom roles

**Lesson files**: `model-view.md`

**Project Milestone**: Log Viewer upgrade — replace plain text with QAbstractTableModel that parses log lines into columns (timestamp, level, message), displayed in QTableView with sortable columns.

**Why It Matters**: Model/View is Qt's most important architectural pattern for data-heavy applications. Instead of stuffing data into widgets, you separate the data (model) from the display (view). The same log data can now be filtered, sorted, and searched through the model layer — impossible with a raw text widget.

---

### Week 8: Filtering, Delegates & Search

- [ ] QSortFilterProxyModel — filtering and sorting without modifying source
- [ ] Custom filter logic — `filterAcceptsRow()`, multi-column filtering
- [ ] QStyledItemDelegate — custom rendering with QPainter
- [ ] Search bar — filtering log entries by text, level, time range

**Lesson files**: `proxy-models.md`, `delegates.md`

**Project Milestone**: Log Viewer complete — filter toolbar (by log level, text search), colored log levels via custom delegate, search/highlight bar, column resizing.

**Why It Matters**: Proxy models are the "killer feature" of Qt's model/view — you add sorting and filtering on top of any model without touching the source data. Custom delegates give you pixel-level control over rendering. This week transforms the log viewer from "displays data" to "helps you find problems."

---

### Week 9: Text Editor — Core

- [ ] Subclassing QPlainTextEdit — line number area with QPainter
- [ ] QUndoStack — undo/redo infrastructure, custom QUndoCommand
- [ ] QSyntaxHighlighter — subclassing, `highlightBlock()`, QTextCharFormat
- [ ] Rule-based highlighting — keyword lists, regex patterns

**Lesson files**: `custom-text-editor.md`, `syntax-highlighting.md`

**Project Milestone**: Text Editor tab — QPlainTextEdit with line numbers painted via QPainter, undo/redo via QUndoStack, basic syntax highlighting for at least one language.

**Why It Matters**: Building a text editor exercises QPainter (line numbers), the undo framework (QUndoStack), and the syntax highlighting system — three distinct Qt subsystems working together. Line numbers require painting in a sidebar widget coordinated with the editor's scroll position, which teaches custom widget composition.

---

### Week 10: Text Editor — Features

- [ ] QFileSystemModel — real-time filesystem monitoring
- [ ] QTreeView — hierarchical display, expand/collapse
- [ ] QTextCursor search — `find()` with QTextDocument, regex support
- [ ] Find/replace dialog — modeless dialog, find next/previous, wrap around
- [ ] Recent files — QSettings + QAction for recent file menu

**Lesson files**: `file-browser.md`, `find-replace.md`

**Project Milestone**: Text Editor complete — file browser sidebar (QTreeView + QFileSystemModel), find/replace dialog with regex, recent files menu, multiple file tabs.

**Why It Matters**: QFileSystemModel + QTreeView gives you a file browser in ~20 lines of setup code — a perfect demonstration of model/view's power. Find/replace is essential for any text editor and teaches QTextCursor manipulation. This week completes the second tab of the DevConsole.

---

### Week 11: Serial Monitor — Connection

- [ ] QSerialPort — opening, configuring (baud rate, data bits, parity, stop bits)
- [ ] QSerialPortInfo — enumerating available ports, vendor/product identification
- [ ] Asynchronous I/O — `readyRead()` signal, non-blocking reads
- [ ] Protocol framing — handling partial reads, line-based vs packet-based

**Lesson files**: `serial-port.md`

**Project Milestone**: Serial Monitor tab — port enumeration dropdown, connect/disconnect, baud rate selection, basic send/receive with QPlainTextEdit display.

**Why It Matters**: Serial communication is the bridge between software and hardware. QSerialPort's signal-based async I/O integrates cleanly with Qt's event loop — no polling, no blocking. Handling partial reads (data arriving in chunks, not complete messages) is a real-world skill that applies to any stream-based protocol.

---

### Week 12: Threading

- [ ] QThread — worker object pattern (`moveToThread`), why not to subclass
- [ ] Thread-safe communication — signals/slots across threads (queued connections)
- [ ] Protecting shared data — QMutex when signals/slots aren't enough
- [ ] Non-blocking UI — keeping the main thread responsive during heavy I/O

**Lesson files**: `threading.md`

**Project Milestone**: Serial Monitor upgrade — move serial I/O to a worker thread via `moveToThread`, non-blocking UI during high-throughput data, thread-safe data buffering.

**Why It Matters**: Threading in Qt is different from raw `std::thread` — the worker object pattern with `moveToThread` is idiomatic. Signals and slots across threads use queued connections automatically, making thread-safe communication almost invisible. This week ensures the Serial Monitor stays responsive even at high data rates.

---

### Week 13: Serial Monitor — Advanced

- [ ] QByteArray — raw byte handling, hex conversions
- [ ] Custom hex widget — dual hex/ASCII display, offset gutter
- [ ] Timestamps — adding receive timestamps to serial data
- [ ] Logging to file — saving serial session data
- [ ] QProcess — starting external processes, capturing stdout/stderr
- [ ] Process state — started/finished/error signals, exit codes

**Lesson files**: `hex-display.md`, `process-launcher.md`

**Project Milestone**: Serial Monitor complete — toggle between hex/ASCII display, timestamps on received data, log session to file. Bonus: QProcess-based command launcher for running build scripts or flashing tools.

**Why It Matters**: Hex display is essential for debugging binary protocols — firmware engineers live here. QProcess rounds out the DevConsole by letting you launch external tools (compilers, flash utilities, test scripts) from within the application, making it a true developer workstation.

---

### Week 14: Cross-Platform & Deployment

- [ ] Platform detection — `Q_OS_WIN`, `Q_OS_MACOS`, `Q_OS_LINUX`
- [ ] Platform-specific code — conditional compilation, platform plugins
- [ ] Native look and feel — Fusion style vs platform native
- [ ] High-DPI scaling — logical vs physical pixels
- [ ] Static vs dynamic linking — trade-offs
- [ ] Deployment tools — windeployqt, macdeployqt, linuxdeployqt
- [ ] Packaging — creating distributable binaries

**Lesson files**: `cross-platform.md`, `deployment.md`

**Project Milestone**: DevConsole polished — platform-specific styling, high-DPI support, packaged for distribution on at least one platform.

**Why It Matters**: "It works on my machine" isn't enough. Deployment is where many Qt projects stumble — the app works in development but crashes on the user's machine because of missing libraries or plugins. Understanding what to ship and how to package it is the difference between a project and a product.

---
up:: [MOC-Programming](../../../01-index/MOC-Programming.md)
#type/learning #source/self-study #status/seed
