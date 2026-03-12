# C++ Qt Learning Path

## Phase 1: Qt Fundamentals (Weeks 1–4)

### Introduction to Qt
- History of Qt — Trolltech to The Qt Company, the open-source journey
- Why Qt? — cross-platform, mature, single codebase for desktop/embedded
- Qt vs Alternatives — GTK, wxWidgets, Dear ImGui, native frameworks
- Qt 6 Module Overview — QtCore, QtWidgets, QtNetwork, QtSerialPort
- Meta-Object System (MOC) — what it generates and why Qt needs it

### Environment Setup
- CMake with Qt 6 — `find_package(Qt6)`, `qt_add_executable`, target linking
- VS Code + clangd — compile_commands.json, extensions, debugging setup
- First app — minimal QApplication + QWidget from the command line

### Qt Object Model
- QObject — the root of Qt's object system
- Parent-Child Ownership — automatic memory management via object trees
- Object Trees — construction order, destruction order, reparenting
- `Q_OBJECT` Macro — what it enables (signals, slots, properties, introspection)

### Qt Type System
- QString — implicit sharing (COW), unicode, conversions to/from std::string
- QList (formerly QVector) — the default container in Qt 6
- QVariant — type-erased value container, when and why to use it
- Implicit Sharing (Copy-on-Write) — how Qt containers avoid unnecessary copies
- QStringView, QLatin1String — avoiding allocations in hot paths (OPTIONAL)

### Testing & Debugging
- QTest Framework — `QCOMPARE`, `QVERIFY`, data-driven tests, `QSignalSpy`
- GUI Testing — simulating clicks and key presses with QTest
- qDebug / qWarning / qCritical — Qt's logging functions
- QLoggingCategory — structured, filterable logging

### Signals & Slots
- Signal-Slot Mechanism — Qt's observer pattern, MOC-generated glue code
- Connection Syntax — old (string-based) vs new (function pointer), always use new
- Custom Signals and Slots — declaring in your own QObject subclasses
- Queued vs Direct Connections — thread-safe communication via the event loop
- Lambda Slots — connecting signals to inline lambdas, capture pitfalls

### Event System
- Event Loop — `QApplication::exec()`, event dispatching, `processEvents()`
- Event Handling — reimplementing `event()`, specific event handlers (`mousePressEvent`, etc.)
- Event Filters — intercepting events before they reach the target object

### Widgets & Layouts
- QWidget — the base class for all visible UI elements
- Layouts — QHBoxLayout, QVBoxLayout, QGridLayout, QFormLayout
- Common Widgets — QPushButton, QLabel, QLineEdit, QComboBox, QCheckBox, QSpinBox
- Size Policies — how widgets negotiate space within layouts

### Main Window Architecture
- QMainWindow — central widget, dock areas, status bar
- Menus and Toolbars — QMenuBar, QToolBar, QAction
- Keyboard Shortcuts — QKeySequence, standard shortcuts
- Status Bar — QStatusBar, temporary and permanent messages

## Phase 2: Building the DevConsole (Weeks 5–14)

### File I/O
- QFile and QTextStream — reading/writing text files
- QSaveFile — atomic writes for safe file saving
- QSettings — persistent application settings (INI/registry)
- QStandardPaths — platform-correct paths for config, data, cache

### Dialogs
- QFileDialog — open/save file dialogs, native vs Qt dialogs
- QMessageBox — information, warning, question dialogs
- QInputDialog — simple single-value input
- Custom Dialogs — subclassing QDialog, accept/reject flow

### Text Display
- QPlainTextEdit — the workhorse for plain text display
- QTextDocument — underlying document model, block structure
- Append-Only Display — efficient log viewing with `appendPlainText()`
- Line Limiting — preventing unbounded memory growth

### Drag & Drop
- QMimeData — packaging data for drag/drop and clipboard
- Drag Events — `dragEnterEvent`, `dropEvent`, accepting specific MIME types
- Drop Indicators — visual feedback during drag operations

### Model/View Programming
- Model/View Architecture — separating data from presentation
- QAbstractTableModel — implementing a custom table model (roles, indexes, headers)
- QTableView — displaying tabular data, column sizing, selection
- Data Roles — Qt::DisplayRole, Qt::ForegroundRole, Qt::ToolTipRole, custom roles

### Proxy Models
- QSortFilterProxyModel — filtering and sorting without modifying source data
- Custom Filter Logic — `filterAcceptsRow()`, multi-column filtering
- Chaining Proxies — stacking multiple proxy models

### Delegates
- QStyledItemDelegate — custom rendering for model/view items
- Custom Painting — `paint()` with QPainter for rich cell rendering
- Custom Editors — `createEditor()`, `setEditorData()`, `setModelData()`

### Custom Text Editor
- Subclassing QPlainTextEdit — line number area with QPainter
- QUndoStack — undo/redo infrastructure, custom QUndoCommand
- Text Cursor Operations — QTextCursor for programmatic editing

### Syntax Highlighting
- QSyntaxHighlighter — subclassing, `highlightBlock()`, QTextCharFormat
- Rule-Based Highlighting — keyword lists, regex patterns, multi-line blocks
- Performance — incremental rehighlighting, avoiding full-document rescans

### File Browser
- QFileSystemModel — real-time filesystem monitoring
- QTreeView — displaying hierarchical data, expand/collapse, selection
- Custom Filtering — hiding files/directories, showing only relevant types

### Find & Replace
- QTextCursor Search — `find()` with QTextDocument, regex support
- Find Dialog — modeless dialog, find next/previous, wrap around
- Replace — single and replace-all, undo integration

### Serial Port Communication
- QSerialPort — opening, configuring (baud rate, data bits, parity, stop bits)
- QSerialPortInfo — enumerating available ports, vendor/product identification
- Asynchronous I/O — `readyRead()` signal, non-blocking reads
- Protocol Framing — handling partial reads, line-based vs packet-based protocols

### Threading
- QThread — worker object pattern (moveToThread), why not to subclass QThread
- Thread-Safe Communication — signals/slots across threads (queued connections)
- QtConcurrent — `run()` for fire-and-forget background tasks (OPTIONAL)
- QMutex, QReadWriteLock — protecting shared data when signals/slots aren't enough (OPTIONAL)

### Hex Display
- QByteArray — raw byte handling, hex conversions
- Custom Hex Widget — dual hex/ASCII display, offset gutter
- Binary Protocols — interpreting structured binary data

### Process Launcher
- QProcess — starting external processes, capturing stdout/stderr
- Process State — started/finished/error signals, exit codes
- Interactive Processes — writing to stdin, reading stdout asynchronously

### Cross-Platform Development
- Platform Detection — `Q_OS_WIN`, `Q_OS_MACOS`, `Q_OS_LINUX`
- Platform-Specific Code — conditional compilation, platform plugins
- Native Look and Feel — platform styles (Fusion, macOS, Windows)
- High-DPI Scaling — logical vs physical pixels, Qt 6 improvements

### Deployment
- Static vs Dynamic Linking — trade-offs for Qt applications
- Deployment Tools — windeployqt, macdeployqt, linuxdeployqt
- CPack with CMake — creating distributable packages (OPTIONAL)
- CI/CD Considerations — building Qt apps in GitHub Actions (OPTIONAL)

---
up:: [MOC-Programming](../../../01-index/MOC-Programming.md)
#type/learning #source/self-study #status/seed
