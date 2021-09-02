/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef MARKDOWN_EDITOR_H
#define MARKDOWN_EDITOR_H

#include <QPlainTextEdit>
#include <QScopedPointer>

#include "colorscheme.h"
#include "markdowndocument.h"
#include "markdowneditortypes.h"

namespace ghostwriter
{
/**
 * Markdown editor having special shortcut key handing and live spell checking.
 */
class MarkdownEditorPrivate;
class MarkdownEditor : public QPlainTextEdit
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MarkdownEditor)

public:
    /**
     * Constructor.
     */
    MarkdownEditor
    (
        MarkdownDocument *textDocument,
        const ColorScheme &colors,
        QWidget *parent = 0
    );

    /**
     * Destructor.
     */
    virtual ~MarkdownEditor();

    /**
     * Draws the block quote and code block backgrounds.
     *
     * Also draws the text cursor.  This has to be done as of Qt 5.8, since
     * this version of Qt chooses the cursor's color based on the
     * editor's background color, rather than the foreground color as it
     * used to in prior versions.  This means that we cannot control the
     * cursor color via the "color" style sheet property.
     */
    void paintEvent(QPaintEvent *event);

    /**
     * Sets the dictionary language to use for spell checking.
     */
    Q_SLOT void setDictionary(const QString &language);

    /**
     * This editor has a preferred layout that is used to center the text
     * editing area in the parent widget, along with aesthetic margins
     * based on the configured EditorWidth setting.
     *
     * Call this method to set the preferred editor layout in the parent
     * widget into which this editor is displayed.
     */
    QLayout *preferredLayout();

    /**
     * Gets whether Hemingway mode is enabled.
     */
    bool hemingwayModeEnabled() const;

    /**
     * Sets whether Hemingway mode is enabled.
     */
    void setHemingWayModeEnabled(bool enabled);

    /**
     * Gets the current focus mode.
     */
    FocusMode focusMode() const;

    /**
     * Sets the focus mode.
     */
    void setFocusMode(FocusMode mode);

    /**
     * Sets the editor color scheme.
     */
    void setColorScheme(const ColorScheme &colors);

    /**
     * Sets the font.
     */
    void setFont(const QString &family, double size);

    /**
     * Sets whether tabs and spaces will be shown.
     */
    void setShowTabsAndSpacesEnabled(bool enabled);

    /**
     * Sets up the margins on the sides of the editor, so that the text
     * area is centered in the window.
     *
     * This method should be called in the parent's resizeEvent(). Note
     * that this theoretically could have been done in the editor's own
     * resizeEvent() method, but unfortunately it is not reliable and can
     * cause crashes with calls to setViewportMargins().
     *
     * The parent window should also call this method after
     * calls to setEditorWidth().
     */
    void setupPaperMargins();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dropEvent(QDropEvent *e);
    void keyPressEvent(QKeyEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);
    void wheelEvent(QWheelEvent *e);

signals:
    /**
     * Notifies listeners that a heading was found in the document in the
     * given QTextBlock and with the given level and heading text.
     */
    void headingFound(int level, const QString &text, QTextBlock block);

    /**
     * Notifies listeners that a heading was discovered to have been removed
     * from the document at the given cursor position.
     */
    void headingRemoved(int position);

    /**
     * Emitted when the user has resumed typing text.
     */
    void typingResumed();

    /**
     * Emitted when the user has stopped typing text.
     * Time emitted is 1000ms since last document update.
     */
    void typingPaused();

    /**
     * Emitted when the user has stopped typing text.
     * Time emitted is scaled per document size up to 1000ms
     * since last document update.
     */
    void typingPausedScaled();

    /**
     * Emitted when the cursor position in the editor has changed.
     * The new position is passed as a parameter.
     */
    void cursorPositionChanged(int position);

    /**
     * Emitted when the user selects text.  The selected text, as
     * well as the cursor position of the beginning and end of the
     * selection in the document, are provided as parameters.
     */
    void textSelected
    (
        const QString &selectedText,
        int selectionStart,
        int selectionEnd
    );

    /**
     * Emitted when the user deselects text (i.e., no text is currently
     * selected in the editor).
     */
    void textDeselected();

    /**
     * Emitted when the user uses CTRL + mouse scroll wheel to zoom in
     * and out, changing the font size.
     */
    void fontSizeChanged(int size);

public slots:
    /**
     * Sets the cursor position in the editor to the given position.
     */
    void navigateDocument(const int position);

    /**
     * Inserts bold formatting.
     */
    void bold();

    /**
     * Inserts italic formatting.
     */
    void italic();

    /**
     * Inserts strikethrough formatting.
     */
    void strikethrough();
    /**
     * Inserts an HTML comment.
     */
    void insertComment();

    /**
     * Formats current line or selected lines as a bullet point
     * list with the asterisk (*) marker.
     */
    void createBulletListWithAsteriskMarker();

    /**
     * Formats current line or selected lines as a bullet point
     * list with the minus (-) marker.
     */
    void createBulletListWithMinusMarker();

    /**
     * Formats current line or selected lines as a bullet point
     * list with the plus (+) marker.
     */
    void createBulletListWithPlusMarker();

    /**
     * Formats current line or selected lines as a numbered
     * list with the period (.) marker.
     */
    void createNumberedListWithPeriodMarker();

    /**
     * Formats current line or selected lines as a numbered
     * list with the parenthesis (')') marker.
     */
    void createNumberedListWithParenthesisMarker();

    /**
     * Formats current line or selected lines as a task list.
     */
    void createTaskList();

    /**
     * Formats current line or selected lines as a block quote.
     */
    void createBlockquote();

    /**
     * Removes block quote formatting from the current line or selected
     * lines.
     */
    void removeBlockquote();

    /**
     * Indents the current line or selected lines.
     */
    void indentText();

    /**
     * Unindents the current line or selected lines.
     */
    void unindentText();

    /**
     * Formats current task list item or selected task list items
     * as complete, checking them with an 'x'.
     */
    bool toggleTaskComplete();

    /**
     * Sets whether large heading sizes are enabled.
     */
    void setEnableLargeHeadingSizes(bool enable);

    /**
     * Sets whether automatching of characters is enabled.
     */
    void setAutoMatchEnabled(bool enable);

    /**
     * Sets whether automatching is enabled for the given opening
     * character.
     */
    void setAutoMatchEnabled(const QChar openingCharacter, bool enabled);

    /**
     * Sets whether bullet points should be automatically cycled with
     * a different bullet point mark (*, -, +) each time a sublist is
     * created.
     */
    void setBulletPointCyclingEnabled(bool enable);

    /**
     * Sets whether emphasized text is underlined or italicized.
     */
    void setUseUnderlineForEmphasis(bool enable);

    /**
     * Sets whether blockquotes should be italicized.
     */
    void setItalicizeBlockquotes(bool enable);

    /**
     * Sets whether to insert spaces instead of tabs when a tab
     * character is inserted.  The number of spaces inserted is
     * determined by the tabulation width.
     */
    void setInsertSpacesForTabs(bool enable);

    /**
     * Sets tabulation width.
     */
    void setTabulationWidth(int width);

    /**
     * Sets the editor width.
     */
    void setEditorWidth(EditorWidth width);

    /**
     * Sets the editor corners.
     */
    void setEditorCorners(InterfaceStyle corners);

    /**
     * Runs the spell checker over the entire document for the
     * live spell checking.
     */
    void runSpellChecker();

    /**
     * Sets whether live spell checking is enabled.
     */
    void setSpellCheckEnabled(const bool enabled);

    /**
     * Increases the font size by 1 pt.
     */
    void increaseFontSize();

    /**
     * Decreases the font size by 1 pt.
     */
    void decreaseFontSize();

protected slots:
    void suggestSpelling(QAction *action);
    void onContentsChanged(int position, int charsAdded, int charsRemoved);
    void onTextBlockRemoved(const QTextBlock &);
    void onSelectionChanged();
    void focusText();
    void checkIfTypingPaused();
    void checkIfTypingPausedScaled();
    void spellCheckFinished(int result);
    void onCursorPositionChanged();

private:
    QScopedPointer<MarkdownEditorPrivate> d_ptr;
};
} // namespace ghostwriter

#endif
