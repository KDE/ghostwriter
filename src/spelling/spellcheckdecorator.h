/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SPELL_CHECK_DECORATOR_H
#define SPELL_CHECK_DECORATOR_H

#include <QEvent>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>

namespace ghostwriter
{
/**
 * A spell checker that does NOT use QSyntaxHighlighter!  You can use this
 * class on top of your own custom QSyntaxHighlighter to have live spell
 * checking and/or a spell checker dialog.
 *
 * WARNING: Instantiate this class only AFTER attaching a QSyntaxHighlighter
 *          to the QPlainTextEdit's underlying document.  This will ensure
 *          that QSyntaxHighlighter doesn't wipe out the live spell check
 *          error highlights.
 */
class SpellCheckDecoratorPrivate;
class SpellCheckDecorator : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SpellCheckDecorator)

public:
    /**
     * Constructor.
     */
    SpellCheckDecorator(QPlainTextEdit *editor);

    /**
     * Destructor.
     */
    ~SpellCheckDecorator();

    /**
     * Returns whether live spell checking is enabled.
     */
    bool liveSpellCheckEnabled() const;

    /**
     * Returns the color used to highlight spelling errors.
     */
    QColor errorColor() const;

public slots:
    /**
     * Sets whether live spell checking is enabled.
     */
    void setLiveSpellCheckEnabled(bool enabled);

    /**
     * Sets the dictionary language to use for spell checking.
     */
    void setDictionary(const QString &language);

    /**
     * Sets the color used to highlight spelling errors.
     */
    void setErrorColor(const QColor &color);

    /**
     * Runs the interactive spell checker dialog.
     */
    void runSpellCheck();

    /**
     * Begins live spell checking on the document.  Call this method once at
     * application startup after show() has been called for the main window
     * (and by extension, any editor whose document has a QSyntaxHighlighter
     * attached to it). This will prevent QSyntaxHighlighter from wiping out the
     * live spell check highlighting, as it tends to rehighlight from scratch
     * multiple times during initialization / first load of text in the document.
     *
     * Note: If live spell checking is disabled (with a call to
     * setLiveSpellCheckEnabled(false), this method will do nothing.
     */
    void startLiveSpellCheck();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<SpellCheckDecoratorPrivate> d_ptr;
};
} // namespace ghostwriter

#endif
