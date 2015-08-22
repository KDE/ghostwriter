/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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

#include <QObject>
#include <QPlainTextEdit>
#include <QString>
#include <QResizeEvent>
#include <QSize>
#include <QLayout>
#include <QGridLayout>
#include <QTextCursor>
#include <QListWidget>
#include <QRegExp>

#include "TextDocument.h"
#include "MarkdownEditorTypes.h"
#include "MarkdownHighlighter.h"
#include "GraphicsFadeEffect.h"
#include "Theme.h"
#include "spelling/dictionary_ref.h"

class MarkdownEditor : public QPlainTextEdit
{
	Q_OBJECT

	public:
        MarkdownEditor
        (
            TextDocument* textDocument,
            MarkdownHighlighter* highlighter,
            QWidget* parent = 0
        );
        virtual ~MarkdownEditor();

        void setDictionary(const DictionaryRef& dictionary);

        /**
         * This editor has a preferred layout that is used to center the text
         * editing area in the parent widget, along with aesthetic margins
         * based on the configured EditorWidth setting.
         *
         * Call this method to set the preferred editor layout in the parent
         * widget into which this editor is displayed.
         */
        QLayout* getPreferredLayout();

        FocusMode getFocusMode();
        void setFocusMode(FocusMode mode);
        void setColorScheme(const MarkdownColorScheme& cs);
        void setAspect(EditorAspect aspect);
        void setFont(const QString& family, double size);

        /**
         * Sets up the margins on the sides of the editor, so that the text
         * area is centered in the window.
         *
         * Parameter is the parent window's width.
         *
         * This method should be called in the parent's resizeEvent(). Note
         * that this theoretically could have been done in the editor's own
         * resizeEvent() method, but unfortunately it is not reliable and can
         * cause crashes with calls to setViewportMargins().
         *
         * The parent window should also call this method after
         * calls to setEditorWidth() and setAspect().
         */
        void setupPaperMargins(int width);

    protected:
        void dragEnterEvent(QDragEnterEvent* e);
        void dropEvent(QDropEvent* e);
        void keyPressEvent(QKeyEvent *e);
        bool eventFilter(QObject* watched, QEvent* event);

    signals:
        void wordCountChanged(int newWordCount);
        void typingResumed();
        void typingPaused();
        void cursorPositionChanged(int position);

    public slots:
        void navigateDocument(const int pos);
        void cut();
        void bold();
        void italic();
        void setEnableLargeHeadingSizes(bool enable);
        void setUseUnderlineForEmphasis(bool enable);
        void setInsertSpacesForTabs(bool enable);
        void setTabulationWidth(int width);
        void setEditorWidth(EditorWidth width);
        void runSpellChecker();
        void setSpellCheckEnabled(const bool enabled);

    private slots:
        void suggestSpelling(QAction* action);
        void onTextChanged(int position, int charsRemoved, int charsAdded);
        void onBlockCountChanged(int newBlockCount);
        void onSelectionChanged();
        void focusText();
        void checkIfTypingPaused();
        void spellCheckFinished(int result);
        void onCursorPositionChanged();

        /*
         * Updates word count.
         */
        void refreshWordCount();

    private:
        TextDocument* textDocument;
        MarkdownHighlighter* highlighter;
        QGridLayout* preferredLayout;
        QAction* addWordToDictionaryAction;
        QAction* checkSpellingAction;
        QString wordUnderMouse;
        QTextCursor cursorForWord;
        DictionaryRef dictionary;
        bool spellCheckEnabled;
        int wordCount;
        QList<QAction*> spellingActions;
        int lastBlockCount;
        FocusMode focusMode;
        QBrush fadeColor;
        EditorAspect aspect;
        bool insertSpacesForTabs;
        int tabWidth;
        EditorWidth editorWidth;
        QRegExp blockquoteRegex;
        QRegExp numberedListRegex;
        QRegExp bulletListRegex;
        QRegExp taskListRegex;
        QHash<QChar, QChar> markupPairs; // Used for auto-insert and pairing.
        MarkdownColorScheme colorScheme;
        bool mouseButtonDown;
        GraphicsFadeEffect* fadeEffect;

        // Timer used to determine when typing has paused.
        QTimer* typingTimer;
        bool typingHasPaused;

        // Use this flag to keep from sending the typingPaused() signal
        // multiple times after it's already been sent the first time after
        // a pause in the user's typing.
        //
        bool typingPausedSignalSent;

        void updateBlockWordCount(QTextBlock& block);
        void handleCarriageReturn();
        bool handleBackspaceKey();
        void handleIndent();
        void handleUnindent();
        void insertBlockquote();
        void removeBlockquote();
        bool insertPairedCharacters(const QChar firstChar);
        void insertFormattingMarkup(const QString& markup);
        bool toggleTaskComplete();
        QString getPriorIndentation();
        QString getPriorMarkdownBlockItemStart(QRegExp& itemRegex);
        int countWords(const QString& text);


};

#endif
