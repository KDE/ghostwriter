/*
 * SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QAction>
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCursor>

#include "dictionary.h"

namespace ghostwriter
{
class SpellChecker : public QDialog
{
    Q_OBJECT

public:
    static void checkDocument(QPlainTextEdit *document, Dictionary *dictionary);

public slots:
    virtual void reject() override;

private slots:
    void suggestionChanged(QListWidgetItem *suggestion);
    void add();
    void ignore();
    void ignoreAll();
    void change();
    void changeAll();

private:
    SpellChecker(QPlainTextEdit *document, Dictionary *dictionary);
    void check();

private:
    Dictionary *m_dictionary;

    QPlainTextEdit *m_document;
    QPlainTextEdit *m_context;
    QLineEdit *m_suggestion;
    QListWidget *m_suggestions;
    QTextCursor m_cursor;
    QTextCursor m_startCursor;

    int m_checkedBlocks;
    int m_totalBlocks;
    bool m_loopAvailable;

    QString m_word;
    QStringList m_ignored;
};
} // namespace ghostwriter

#endif
