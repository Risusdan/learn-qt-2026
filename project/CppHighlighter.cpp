// Week 9 — Text Editor Core

#include "CppHighlighter.h"

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // --- Keyword format (dark blue, bold) ---
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(0, 0, 160));
    keywordFormat.setFontWeight(QFont::Bold);

    const QStringList keywords = {
        "alignas",    "alignof",    "and",        "and_eq",
        "asm",        "auto",       "bitand",     "bitor",
        "break",      "case",       "catch",      "class",
        "compl",      "concept",    "const",      "consteval",
        "constexpr",  "constinit",  "const_cast", "continue",
        "co_await",   "co_return",  "co_yield",   "decltype",
        "default",    "delete",     "do",         "dynamic_cast",
        "else",       "enum",       "explicit",   "export",
        "extern",     "false",      "for",        "friend",
        "goto",       "if",         "inline",     "mutable",
        "namespace",  "new",        "noexcept",   "not",
        "not_eq",     "nullptr",    "operator",   "or",
        "or_eq",      "override",   "private",    "protected",
        "public",     "register",   "reinterpret_cast",
        "requires",   "return",     "sizeof",     "static",
        "static_assert", "static_cast", "struct", "switch",
        "template",   "this",       "throw",      "true",
        "try",        "typedef",    "typeid",     "typename",
        "union",      "using",      "virtual",    "volatile",
        "while",      "xor",        "xor_eq",
        // Qt-specific
        "Q_OBJECT",   "Q_PROPERTY", "Q_INVOKABLE",
        "signals",    "slots",      "emit",
        "foreach",    "forever",
    };

    for (const QString &word : keywords) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\\b%1\\b").arg(word));
        rule.format = keywordFormat;
        m_rules.append(rule);
    }

    // --- Type format (dark magenta) ---
    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor(128, 0, 128));

    const QStringList types = {
        "bool",       "char",       "char8_t",    "char16_t",
        "char32_t",   "double",     "float",      "int",
        "long",       "short",      "signed",     "unsigned",
        "void",       "wchar_t",    "size_t",     "int8_t",
        "int16_t",    "int32_t",    "int64_t",    "uint8_t",
        "uint16_t",   "uint32_t",   "uint64_t",
        "string",     "vector",     "map",        "set",
        "array",      "optional",   "variant",    "unique_ptr",
        "shared_ptr", "pair",       "tuple",
        // Qt types
        "QString",    "QStringList","QList",      "QMap",
        "QSet",       "QHash",      "QVariant",   "QObject",
        "QWidget",    "QMainWindow","QDialog",    "QAction",
        "QLabel",     "QLineEdit",  "QPushButton","QComboBox",
        "QCheckBox",  "QRadioButton","QTextEdit", "QPlainTextEdit",
        "QTableView", "QTreeView",  "QListView",
        "QAbstractTableModel", "QAbstractItemModel",
        "QSortFilterProxyModel", "QStyledItemDelegate",
        "QFile",      "QTextStream","QSettings",  "QTimer",
        "QColor",     "QFont",      "QRect",      "QSize",
        "QPoint",     "QByteArray", "QUrl",
    };

    for (const QString &word : types) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\\b%1\\b").arg(word));
        rule.format = typeFormat;
        m_rules.append(rule);
    }

    // --- Single-line comment (dark green) ---
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(0, 128, 0));

    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
        rule.format = commentFormat;
        m_rules.append(rule);
    }

    // --- String literals (dark red) ---
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(163, 21, 21));

    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\"(?:[^\"\\\\]|\\\\.)*\""));
        rule.format = stringFormat;
        m_rules.append(rule);
    }

    // Character literals
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("'(?:[^'\\\\]|\\\\.)*'"));
        rule.format = stringFormat;
        m_rules.append(rule);
    }

    // --- Preprocessor (dark cyan) ---
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(0, 128, 128));

    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("^\\s*#[^\n]*"));
        rule.format = preprocessorFormat;
        m_rules.append(rule);
    }

    // --- Function calls (blue, italic) ---
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(0, 0, 200));
    functionFormat.setFontItalic(true);

    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\s*\\()"));
        rule.format = functionFormat;
        m_rules.append(rule);
    }

    // --- Numbers (dark orange) ---
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(200, 120, 0));

    {
        HighlightingRule rule;
        // Matches integers, hex, floats
        rule.pattern = QRegularExpression(
            QStringLiteral("\\b(?:0[xX][0-9a-fA-F]+|0[bB][01]+|\\d+\\.?\\d*(?:[eE][+-]?\\d+)?[fFlLuU]*)\\b"));
        rule.format = numberFormat;
        m_rules.append(rule);
    }

    // --- Multi-line comment setup ---
    m_multiLineCommentFormat = commentFormat;
    m_commentStartExpr = QRegularExpression(QStringLiteral("/\\*"));
    m_commentEndExpr   = QRegularExpression(QStringLiteral("\\*/"));
}

void CppHighlighter::highlightBlock(const QString &text)
{
    // Apply single-line rules
    for (const HighlightingRule &rule : std::as_const(m_rules)) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // --- Multi-line comment handling ---
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        QRegularExpressionMatch startMatch = m_commentStartExpr.match(text);
        startIndex = startMatch.hasMatch() ? startMatch.capturedStart() : -1;
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch =
            m_commentEndExpr.match(text, startIndex + 2);

        int commentLength;
        if (!endMatch.hasMatch()) {
            // Comment extends to end of block (and beyond)
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endMatch.capturedEnd() - startIndex;
        }

        setFormat(startIndex, commentLength, m_multiLineCommentFormat);

        // Look for next /* after current */ ends
        QRegularExpressionMatch nextStart =
            m_commentStartExpr.match(text, startIndex + commentLength);
        startIndex = nextStart.hasMatch() ? nextStart.capturedStart() : -1;
    }
}
