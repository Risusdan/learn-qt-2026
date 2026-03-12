// Week 9 — Text Editor Core

#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class CppHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CppHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QList<HighlightingRule> m_rules;

    // Multi-line comment delimiters
    QRegularExpression m_commentStartExpr;
    QRegularExpression m_commentEndExpr;
    QTextCharFormat m_multiLineCommentFormat;
};

#endif // CPPHIGHLIGHTER_H
