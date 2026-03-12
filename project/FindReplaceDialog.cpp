// Week 10 — Text Editor Features

#include "FindReplaceDialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>
#include <QVBoxLayout>

FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Find and Replace"));
    setMinimumWidth(400);

    // --- Input fields ---
    auto *findLabel    = new QLabel(tr("Find:"), this);
    auto *replaceLabel = new QLabel(tr("Replace:"), this);
    m_findEdit    = new QLineEdit(this);
    m_replaceEdit = new QLineEdit(this);

    auto *inputLayout = new QGridLayout;
    inputLayout->addWidget(findLabel,    0, 0);
    inputLayout->addWidget(m_findEdit,    0, 1);
    inputLayout->addWidget(replaceLabel, 1, 0);
    inputLayout->addWidget(m_replaceEdit, 1, 1);

    // --- Options ---
    m_caseSensitive = new QCheckBox(tr("Case Sensitive"), this);
    m_wholeWords    = new QCheckBox(tr("Whole Words"), this);
    m_useRegex      = new QCheckBox(tr("Use Regex"), this);

    auto *optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(m_caseSensitive);
    optionsLayout->addWidget(m_wholeWords);
    optionsLayout->addWidget(m_useRegex);

    // --- Buttons ---
    auto *findNextBtn     = new QPushButton(tr("Find &Next"), this);
    auto *findPrevBtn     = new QPushButton(tr("Find &Previous"), this);
    auto *replaceBtn      = new QPushButton(tr("&Replace"), this);
    auto *replaceAllBtn   = new QPushButton(tr("Replace &All"), this);
    auto *closeBtn        = new QPushButton(tr("Close"), this);

    findNextBtn->setDefault(true);

    auto *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(findNextBtn);
    buttonLayout->addWidget(findPrevBtn);
    buttonLayout->addWidget(replaceBtn);
    buttonLayout->addWidget(replaceAllBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeBtn);

    // --- Status label ---
    m_statusLabel = new QLabel(this);

    // --- Main layout ---
    auto *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(inputLayout);
    leftLayout->addLayout(optionsLayout);
    leftLayout->addWidget(m_statusLabel);
    leftLayout->addStretch();

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout, /*stretch=*/1);
    mainLayout->addLayout(buttonLayout);

    // --- Connections ---
    connect(findNextBtn,   &QPushButton::clicked, this, &FindReplaceDialog::onFindNext);
    connect(findPrevBtn,   &QPushButton::clicked, this, &FindReplaceDialog::onFindPrevious);
    connect(replaceBtn,    &QPushButton::clicked, this, &FindReplaceDialog::onReplace);
    connect(replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAll);
    connect(closeBtn,      &QPushButton::clicked, this, &QDialog::hide);
}

void FindReplaceDialog::setTextEdit(QPlainTextEdit *editor)
{
    m_editor = editor;
}

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------

bool FindReplaceDialog::findText(bool backward)
{
    if (!m_editor || m_findEdit->text().isEmpty()) {
        return false;
    }

    QTextDocument::FindFlags flags;
    if (backward) {
        flags |= QTextDocument::FindBackward;
    }
    if (m_caseSensitive->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_wholeWords->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }

    bool found = false;

    if (m_useRegex->isChecked()) {
        QRegularExpression re(m_findEdit->text());
        if (m_caseSensitive->isChecked()) {
            re.setPatternOptions(QRegularExpression::NoPatternOption);
        } else {
            re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        }
        found = m_editor->find(re, flags);
    } else {
        found = m_editor->find(m_findEdit->text(), flags);
    }

    if (!found) {
        // Wrap around: move cursor to start (or end) and try once more
        QTextCursor cursor = m_editor->textCursor();
        cursor.movePosition(backward ? QTextCursor::End : QTextCursor::Start);
        m_editor->setTextCursor(cursor);

        if (m_useRegex->isChecked()) {
            QRegularExpression re(m_findEdit->text());
            if (m_caseSensitive->isChecked()) {
                re.setPatternOptions(QRegularExpression::NoPatternOption);
            } else {
                re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            }
            found = m_editor->find(re, flags);
        } else {
            found = m_editor->find(m_findEdit->text(), flags);
        }

        if (found) {
            m_statusLabel->setText(tr("Wrapped around"));
        } else {
            m_statusLabel->setText(tr("Not found"));
        }
    } else {
        m_statusLabel->setText(tr("Found"));
    }

    return found;
}

void FindReplaceDialog::onFindNext()
{
    findText(false);
}

void FindReplaceDialog::onFindPrevious()
{
    findText(true);
}

// ---------------------------------------------------------------------------
// Replace
// ---------------------------------------------------------------------------

void FindReplaceDialog::onReplace()
{
    if (!m_editor) {
        return;
    }

    // If there's a selection that matches the search term, replace it
    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection()) {
        cursor.insertText(m_replaceEdit->text());
    }

    // Find the next occurrence
    findText(false);
}

void FindReplaceDialog::onReplaceAll()
{
    if (!m_editor || m_findEdit->text().isEmpty()) {
        return;
    }

    // Move cursor to the beginning
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(cursor);

    int count = 0;

    // Group all replacements into a single undo step
    cursor.beginEditBlock();

    QTextDocument::FindFlags flags;
    if (m_caseSensitive->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_wholeWords->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }

    while (true) {
        bool found = false;

        if (m_useRegex->isChecked()) {
            QRegularExpression re(m_findEdit->text());
            if (m_caseSensitive->isChecked()) {
                re.setPatternOptions(QRegularExpression::NoPatternOption);
            } else {
                re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            }
            found = m_editor->find(re, flags);
        } else {
            found = m_editor->find(m_findEdit->text(), flags);
        }

        if (!found) {
            break;
        }

        // Replace the found text
        QTextCursor replaceCursor = m_editor->textCursor();
        replaceCursor.insertText(m_replaceEdit->text());
        ++count;
    }

    cursor.endEditBlock();

    m_statusLabel->setText(tr("%1 replacement(s)").arg(count));
}
