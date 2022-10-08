/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
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
     * Returns the color used to highlight spelling errors.
     */
    QColor errorColor() const;

public slots:
    /**
     * Updates internal state on spell check configuration change.
     */
    void settingsChanged();

    /**
     * Sets the color used to highlight spelling errors.
     */
    void setErrorColor(const QColor &color);

    /**
     * Begins automatic spell checking on the document.  Call this method once at
     * application startup after show() has been called for the main window
     * (and by extension, any editor whose document has a QSyntaxHighlighter
     * attached to it). This will prevent QSyntaxHighlighter from wiping out the
     * live spell check highlighting, as it tends to rehighlight from scratch
     * multiple times during initialization / first load of text in the document.
     *
     * Note: If automatic spell checking is disabled (via Sonnet configuration),
     * this method will do nothing.
     */
    void startLiveSpellCheck();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QScopedPointer<SpellCheckDecoratorPrivate> d_ptr;
};
} // namespace ghostwriter

#endif
