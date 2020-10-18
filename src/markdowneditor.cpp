/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
 * Copyright (C) Dmitry Shachnev 2012
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

#include <QApplication>
#include <QChar>
#include <QColor>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QHeaderView>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QScreen>
#include <QScrollBar>
#include <QString>
#include <QTextBoundaryFinder>
#include <QTextStream>
#include <QTimer>
#include <QUrl>

#include <QGridLayout>
#include <QLayout>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QSize>
#include <QString>
#include <QTextCursor>

#include "cmarkgfmapi.h"
#include "markdowneditor.h"
#include "markdownhighlighter.h"
#include "markdownstates.h"
#include "spelling/dictionary_manager.h"
#include "spelling/dictionary_ref.h"
#include "spelling/spell_checker.h"

#define GW_TEXT_FADE_FACTOR 1.5

namespace ghostwriter
{
class MarkdownEditorPrivate
{
    Q_DECLARE_PUBLIC(MarkdownEditor)

public:
    MarkdownEditorPrivate(MarkdownEditor *q_ptr)
        : q_ptr(q_ptr),
          dictionary(DictionaryManager::instance().requestDictionary())
    {
        ;
    }

    ~MarkdownEditorPrivate()
    {
        ;
    }

    typedef enum {
        BlockTypeNone,
        BlockTypeQuote,
        BlockTypeCode
    } BlockType;

    MarkdownEditor *q_ptr;

    MarkdownDocument *textDocument;
    MarkdownHighlighter *highlighter;
    QGridLayout *preferredLayout;
    QAction *addWordToDictionaryAction;
    QAction *checkSpellingAction;
    QString wordUnderMouse;
    QTextCursor cursorForWord;
    DictionaryRef dictionary;
    bool spellCheckEnabled;
    bool autoMatchEnabled;
    bool bulletPointCyclingEnabled;
    QList<QAction *> spellingActions;
    bool hemingwayModeEnabled;
    FocusMode focusMode;
    QBrush fadeColor;
    QColor blockColor;
    bool insertSpacesForTabs;
    int tabWidth;
    EditorWidth editorWidth;
    InterfaceStyle editorCorners;
    QRegularExpression emptyBlockquoteRegex;
    QRegularExpression emptyNumberedListRegex;
    QRegularExpression emptyBulletListRegex;
    QRegularExpression emptyTaskListRegex;
    QRegularExpression blockquoteRegex;
    QRegularExpression numberedListRegex;
    QRegularExpression bulletListRegex;
    QRegularExpression taskListRegex;

    // Used for auto-insert and pairing.
    QHash<QChar, QChar> markupPairs;

    // Used for filtering paired characters.
    QHash<QChar, bool> autoMatchFilter;

    // Used for determining if whitespace is allowed between paired
    // characters when autopairing.
    QHash<QChar, QChar> nonEmptyMarkupPairs;

    bool mouseButtonDown;
    QColor cursorColor;
    bool textCursorVisible;
    QTimer *cursorBlinkTimer;

    // Timers used to determine when typing has paused.
    QTimer *typingTimer;
    QTimer *scaledTypingTimer;

    bool typingHasPaused;
    bool scaledTypingHasPaused;

    // Use these flags to keep from sending the typingPaused() and
    // typingPausedScaled() signals multiple times after they have
    // already been sent the first time after a pause in the user's
    // typing.
    //
    bool typingPausedSignalSent;
    bool typingPausedScaledSignalSent;

    void toggleCursorBlink();
    void parseDocument();

    void handleCarriageReturn();
    bool handleBackspaceKey();
    void insertPrefixForBlocks(const QString &prefix);
    void createNumberedList(const QChar marker);
    bool insertPairedCharacters(const QChar firstChar);
    bool handleEndPairCharacterTyped(const QChar ch);
    bool handleWhitespaceInEmptyMatch(const QChar whitespace);
    void insertFormattingMarkup(const QString &markup);
    QString priorIndentation();
    QString priorMarkdownBlockItemStart
    (
        const QRegularExpression &itemRegex,
        QRegularExpressionMatch &match
    );

    bool atBlockAreaStart(const QTextBlock &block, BlockType &type) const;
    bool atBlockAreaEnd(const QTextBlock &block, const BlockType type) const;
    bool atCodeBlockStart(const QTextBlock &block) const;
    bool atCodeBlockEnd(const QTextBlock &block) const;
    bool isBlockquote(const QTextBlock &block) const;
};

MarkdownEditor::MarkdownEditor
(
    MarkdownDocument *textDocument,
    const ColorScheme &colors,
    QWidget *parent
)
    : QPlainTextEdit(parent),
      d_ptr(new MarkdownEditorPrivate(this))
{
    d_func()->textDocument = textDocument;
    d_func()->autoMatchEnabled = true;
    d_func()->bulletPointCyclingEnabled = true;
    d_func()->mouseButtonDown = false;

    this->setDocument(textDocument);
    this->setAcceptDrops(true);

    d_func()->preferredLayout = new QGridLayout();
    d_func()->preferredLayout->setSpacing(0);
    d_func()->preferredLayout->setMargin(0);
    d_func()->preferredLayout->setContentsMargins(0, 0, 0, 0);
    d_func()->preferredLayout->addWidget(this, 0, 0);

    d_func()->blockquoteRegex.setPattern("^ {0,3}(>\\s*)+");
    d_func()->numberedListRegex.setPattern("^\\s*([0-9]+)[.)]\\s+");
    d_func()->bulletListRegex.setPattern("^\\s*[+*-]\\s+");
    d_func()->taskListRegex.setPattern("^\\s*[-*+] \\[([x ])\\]\\s+");
    d_func()->emptyBlockquoteRegex.setPattern("^ {0,3}(>\\s*)+$");
    d_func()->emptyNumberedListRegex.setPattern("^\\s*([0-9]+)[.)]\\s+$");
    d_func()->emptyBulletListRegex.setPattern("^\\s*[+*-]\\s+$");
    d_func()->emptyTaskListRegex.setPattern("^\\s*[-*+] \\[([x ])\\]\\s+$");

    this->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setShowTabsAndSpacesEnabled(true);

    // Make sure QPlainTextEdit does not draw a cursor.  (We'll paint it manually.)
    this->setCursorWidth(0);

    this->setCenterOnScroll(true);
    this->ensureCursorVisible();
    d_func()->spellCheckEnabled = false;
    this->installEventFilter(this);
    this->viewport()->installEventFilter(this);
    d_func()->hemingwayModeEnabled = false;
    d_func()->focusMode = FocusModeDisabled;
    d_func()->insertSpacesForTabs = false;
    this->setTabulationWidth(4);
    d_func()->editorWidth = EditorWidthMedium;
    d_func()->editorCorners = InterfaceStyleRounded;

    d_func()->markupPairs.insert('"', '"');
    d_func()->markupPairs.insert('\'', '\'');
    d_func()->markupPairs.insert('(', ')');
    d_func()->markupPairs.insert('[', ']');
    d_func()->markupPairs.insert('{', '}');
    d_func()->markupPairs.insert('*', '*');
    d_func()->markupPairs.insert('_', '_');
    d_func()->markupPairs.insert('`', '`');
    d_func()->markupPairs.insert('<', '>');

    // Set automatching for the above markup pairs to be
    // enabled by default.
    //
    d_func()->autoMatchFilter.insert('"', true);
    d_func()->autoMatchFilter.insert('\'', true);
    d_func()->autoMatchFilter.insert('(', true);
    d_func()->autoMatchFilter.insert('[', true);
    d_func()->autoMatchFilter.insert('{', true);
    d_func()->autoMatchFilter.insert('*', true);
    d_func()->autoMatchFilter.insert('_', true);
    d_func()->autoMatchFilter.insert('`', true);
    d_func()->autoMatchFilter.insert('<', true);

    d_func()->nonEmptyMarkupPairs.insert('*', '*');
    d_func()->nonEmptyMarkupPairs.insert('_', '_');
    d_func()->nonEmptyMarkupPairs.insert('<', '>');

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(this->document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(onContentsChanged(int, int, int)));
    connect(this->document(), SIGNAL(textBlockRemoved(const QTextBlock &)), this, SLOT(onTextBlockRemoved(const QTextBlock &)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

    d_func()->highlighter = new MarkdownHighlighter(this, colors);
    d_func()->addWordToDictionaryAction = new QAction(tr("Add word to dictionary"), this);
    d_func()->checkSpellingAction = new QAction(tr("Check spelling..."), this);

    d_func()->typingPausedSignalSent = true;
    d_func()->typingHasPaused = true;

    d_func()->typingTimer = new QTimer(this);
    connect
    (
        d_func()->typingTimer,
        SIGNAL(timeout()),
        this,
        SLOT(checkIfTypingPaused())
    );
    d_func()->typingTimer->start(1000);

    d_func()->typingPausedScaledSignalSent = true;
    d_func()->scaledTypingHasPaused = true;

    d_func()->scaledTypingTimer = new QTimer(this);
    connect
    (
        d_func()->scaledTypingTimer,
        SIGNAL(timeout()),
        this,
        SLOT(checkIfTypingPausedScaled())
    );
    d_func()->scaledTypingTimer->start(1000);

    this->setColorScheme(colors);
    d_func()->textCursorVisible = true;

    d_func()->cursorBlinkTimer = new QTimer(this);
    this->connect
    (
        d_func()->cursorBlinkTimer,
        &QTimer::timeout,
    [this]() {
        d_func()->toggleCursorBlink();
    }
    );
    d_func()->cursorBlinkTimer->start(500);
}

MarkdownEditor::~MarkdownEditor()
{
    ;
}

void MarkdownEditor::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    QRect viewportRect = viewport()->rect();
    painter.fillRect(viewportRect, Qt::transparent);

    QPointF offset(contentOffset());
    QTextBlock block = firstVisibleBlock();

    bool firstVisible = true;

    QRectF blockAreaRect; // Code or block quote rect.
    bool inBlockArea = false;
    MarkdownEditorPrivate::BlockType blockType = MarkdownEditorPrivate::BlockTypeNone;
    bool clipTop = false;
    bool drawBlock = false;
    int dy = 0;
    bool done = false;

    int cornerRadius = 5;

    if (InterfaceStyleSquare == d_func()->editorCorners) {
        cornerRadius = 0;
    }

    // Draw text block area backgrounds for code blocks and block quotes.
    // The backgrounds are drawn per each block area (consisting of multiple
    // text blocks or lines), rather than one rectangle area per text block/
    // line in case there are margins between each text block.  This way,
    // the background will extend to cover the margins between text blocks
    // as well.
    //
    // NOTE: Algorithm for looping through text blocks is a partial lift from
    //       Qt's QPlainTextEdit paintEvent() code. Please refer to the
    //       LGPL v. 3 license for the original Qt code.
    //
    while (block.isValid() && !done) {
        QRectF r = this->blockBoundingRect(block).translated(offset);

        // If the block begins a new text block area...
        if (!inBlockArea && d_func()->atBlockAreaStart(block, blockType)) {
            blockAreaRect = r;
            dy = 0;
            inBlockArea = true;

            MarkdownEditorPrivate::BlockType prevType;

            // If this is the first visible block within the viewport
            // and if the previous block is part of the text block area,
            // then the rectangle to draw for the block area will have
            // its top clipped by the viewport and will need to be
            // drawn specially.
            //
            if
            (
                firstVisible
                && d_func()->atBlockAreaStart(block.previous(), prevType)
                && (blockType == prevType)
            ) {
                clipTop = true;
            }
        }
        // Else if the block ends a text block area...
        else if (inBlockArea && d_func()->atBlockAreaEnd(block, blockType)) {
            drawBlock = true;
            inBlockArea = false;
            blockAreaRect.setHeight(dy);
        }

        // If the block is at the end of the document and ends a text
        // block area...
        //
        if (inBlockArea && (block == this->document()->lastBlock())) {
            drawBlock = true;
            inBlockArea = false;
            dy += r.height();
            blockAreaRect.setHeight(dy);
        }

        offset.ry() += r.height();
        dy += r.height();

        // If this is the last text block visible within the viewport...
        if (offset.y() > viewportRect.height()) {
            if (inBlockArea) {
                blockAreaRect.setHeight(dy);
                drawBlock = true;
            }

            // Finished drawing.
            done = true;
        }

        if (drawBlock) {
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(d_func()->blockColor));

            // If the first visible block is "clipped" such that the previous block
            // is part of the text block area, then only draw a rectangle with the
            // bottom corners rounded, and with the top corners square to reflect
            // that the first visible block is part of a larger block of text.
            //
            if (clipTop) {
                QPainterPath path;
                path.setFillRule(Qt::WindingFill);
                path.addRoundedRect(blockAreaRect, cornerRadius, cornerRadius);
                qreal adjustedHeight = blockAreaRect.height() / 2;
                path.addRect(blockAreaRect.adjusted(0, 0, 0, -adjustedHeight));
                painter.drawPath(path.simplified());
                clipTop = false;
            }
            // Else draw the entire rectangle with all corners rounded.
            else {
                painter.drawRoundedRect(blockAreaRect, cornerRadius, cornerRadius);
            }

            drawBlock = false;
        }

        // This fixes the RTL bug of QPlainTextEdit
        // https://bugreports.qt.io/browse/QTBUG-7516.
        //
        // Credit goes to Patrizio Bekerle (qmarkdowntextedit) for discovering
        // this workaround.
        //
        if (block.text().isRightToLeft()) {
            QTextLayout *layout = block.layout();
            QTextOption opt = document()->defaultTextOption();
            opt = QTextOption(Qt::AlignRight);
            opt.setTextDirection(Qt::RightToLeft);
            layout->setTextOption(opt);
        }

        block = block.next();
        firstVisible = false;
    }

    painter.end();

    // Draw the visible editor text.
    QPlainTextEdit::paintEvent(event);

    // Draw the text cursor/caret.
    if (d_func()->textCursorVisible && this->hasFocus()) {
        // Get the cursor rect so that we have the ideal height for it,
        // and then set it to be 2 pixels wide.  (The width will be zero,
        // because we set it to be that in the constructor so that
        // QPlainTextEdit will not draw another cursor underneath this one.)
        //
        QRect r = cursorRect();
        r.setWidth(2);

        QPainter painter(viewport());
        painter.fillRect(r, QBrush(d_func()->cursorColor));
        painter.end();
    }
}

void MarkdownEditor::setDictionary(const QString &language)
{
    d_func()->dictionary = DictionaryManager::instance().requestDictionary(language);
    d_func()->highlighter->setDictionary(d_func()->dictionary);
}

QLayout *MarkdownEditor::preferredLayout()
{
    return d_func()->preferredLayout;
}

bool MarkdownEditor::hemingwayModeEnabled() const
{
    return d_func()->hemingwayModeEnabled;
}

/**
 * Sets whether Hemingway mode is enabled.
 */
void MarkdownEditor::setHemingWayModeEnabled(bool enabled)
{
    d_func()->hemingwayModeEnabled = enabled;
}

FocusMode MarkdownEditor::focusMode()
{
    return d_func()->focusMode;
}

void MarkdownEditor::setFocusMode(FocusMode mode)
{
    d_func()->focusMode = mode;

    if (FocusModeDisabled != mode) {
        connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(focusText()));
        connect(this, SIGNAL(selectionChanged()), this, SLOT(focusText()));
        connect(this, SIGNAL(textChanged()), this, SLOT(focusText()));
        this->focusText();
    } else {
        disconnect(this, SIGNAL(cursorPositionChanged()), this, SLOT(focusText()));
        disconnect(this, SIGNAL(selectionChanged()), this, SLOT(focusText()));
        disconnect(this, SIGNAL(textChanged()), this, SLOT(focusText()));
        this->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
}

void MarkdownEditor::setColorScheme
(
    const ColorScheme &colors
)
{
    d_func()->highlighter->setColorScheme(colors);
    d_func()->cursorColor = colors.cursor;
    d_func()->blockColor = colors.foreground;
    d_func()->blockColor.setAlpha(10);

    QColor fadedForegroundColor = colors.foreground;
    fadedForegroundColor.setAlpha(100);

    d_func()->fadeColor = QBrush(fadedForegroundColor);
    this->focusText();
}

void MarkdownEditor::setFont(const QString &family, double pointSize)
{
    QFont font(family, pointSize);
    QPlainTextEdit::setFont(font);
    d_func()->highlighter->setFont(family, pointSize);
    setTabulationWidth(d_func()->tabWidth);
}

void MarkdownEditor::setShowTabsAndSpacesEnabled(bool enabled)
{
    QTextOption option = d_func()->textDocument->defaultTextOption();

    if (enabled) {
        option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
    } else {
        option.setFlags(option.flags() & ~QTextOption::ShowTabsAndSpaces);
    }

    d_func()->textDocument->setDefaultTextOption(option);
}

void MarkdownEditor::setupPaperMargins(int width)
{
    if (EditorWidthFull == d_func()->editorWidth) {
        d_func()->preferredLayout->setContentsMargins(0, 0, 0, 0);
        setViewportMargins(0, 0, 0, 0);

        return;
    }

    int screenWidth = QGuiApplication::primaryScreen()->size().width();
    int proposedEditorWidth = width;
    int margin = 0;

    switch (d_func()->editorWidth) {
    case EditorWidthNarrow:
        proposedEditorWidth = screenWidth / 3;
        break;
    case EditorWidthMedium:
        proposedEditorWidth = screenWidth / 2;
        break;
    case EditorWidthWide:
        proposedEditorWidth = 2 * (screenWidth / 3);
        break;
    default:
        break;
    }

    if (proposedEditorWidth <= width) {
        margin = (width - proposedEditorWidth) / 2;
    }

    d_func()->preferredLayout->setContentsMargins(0, 0, 0, 0);
    this->setViewportMargins(margin, 20, margin, 0);
}

void MarkdownEditor::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    } else {
        QPlainTextEdit::dragEnterEvent(e);
    }
}

void MarkdownEditor::dragMoveEvent(QDragMoveEvent *e)
{
    e->acceptProposedAction();
}

void MarkdownEditor::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void MarkdownEditor::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls() && (e->mimeData()->urls().size() == 1)) {
        e->acceptProposedAction();

        QUrl url = e->mimeData()->urls().first();
        QString path = url.toLocalFile();
        bool isRelativePath = false;

        QFileInfo fileInfo(path);
        QString fileExtension = fileInfo.suffix().toLower();

        QTextCursor dropCursor = cursorForPosition(e->pos());

        // If the file extension indicates an image type, then insert an
        // image link into the text.
        if
        (
            (fileExtension == "jpg") ||
            (fileExtension == "jpeg") ||
            (fileExtension == "gif") ||
            (fileExtension == "bmp") ||
            (fileExtension == "png") ||
            (fileExtension == "tif") ||
            (fileExtension == "tiff") ||
            (fileExtension == "svg")
        ) {
            if (!d_func()->textDocument->isNew()) {
                QFileInfo docInfo(d_func()->textDocument->filePath());

                if (docInfo.exists()) {
                    path = docInfo.dir().relativeFilePath(path);
                    isRelativePath = true;
                }
            }

            if (!isRelativePath) {
                path = url.toString();
            }

            dropCursor.insertText(QString("![](%1)").arg(path));

            // We have to call the super class so that clean up occurs,
            // otherwise the editor's cursor will freeze.  We also have to use
            // a dummy drop event with dummy MIME data, otherwise the parent
            // class will insert the file path into the document.
            //
            QMimeData *dummyMimeData = new QMimeData();
            dummyMimeData->setText("");
            QDropEvent *dummyEvent =
                new QDropEvent
            (
                e->pos(),
                e->possibleActions(),
                dummyMimeData,
                e->mouseButtons(),
                e->keyboardModifiers()
            );
            QPlainTextEdit::dropEvent(dummyEvent);

            delete dummyEvent;
            delete dummyMimeData;
        }
        // Else insert URL path as normal, using the parent class.
        else {
            QPlainTextEdit::dropEvent(e);
        }
    } else {
        QPlainTextEdit::dropEvent(e);
    }
}

/*
* This method contains a code snippet that was lifted and modified from ReText
*/
void MarkdownEditor::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();

    QTextCursor cursor(this->textCursor());

    switch (key) {
    case Qt::Key_Return:
        if (!cursor.hasSelection()) {
            if (e->modifiers() & Qt::ShiftModifier) {
                // Insert Markdown-style line break
                cursor.insertText("  ");
                d_func()->highlighter->rehighlightBlock(cursor.block());
            }

            if (e->modifiers() & Qt::ControlModifier) {
                cursor.insertText("\n");
            } else {
                d_func()->handleCarriageReturn();
            }
        } else {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    case Qt::Key_Delete:
        if (!d_func()->hemingwayModeEnabled) {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    case Qt::Key_Backspace:
        if (!d_func()->hemingwayModeEnabled) {
            if (!d_func()->handleBackspaceKey()) {
                QPlainTextEdit::keyPressEvent(e);
            }
        }
        break;
    case Qt::Key_Tab:
        if (!d_func()->handleWhitespaceInEmptyMatch('\t')) {
            indentText();
        }
        break;
    case Qt::Key_Backtab:
        unindentText();
        break;
    case Qt::Key_Space:
        if (!d_func()->handleWhitespaceInEmptyMatch(' ')) {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    default:
        if (e->text().size() == 1) {
            QChar ch = e->text().at(0);

            if (!d_func()->handleEndPairCharacterTyped(ch) && !d_func()->insertPairedCharacters(ch)) {
                QPlainTextEdit::keyPressEvent(e);
            }
        } else {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    }
}

bool MarkdownEditor::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        d_func()->mouseButtonDown = true;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        d_func()->mouseButtonDown = false;
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        d_func()->mouseButtonDown = true;
    }

    if (event->type() != QEvent::ContextMenu || !d_func()->spellCheckEnabled || this->isReadOnly()) {
        return QPlainTextEdit::eventFilter(watched, event);
    } else {
        // Check spelling of text block under mouse
        QContextMenuEvent *contextEvent = static_cast<QContextMenuEvent *>(event);

        // If the context menu event was triggered by pressing the menu key,
        // use the current text cursor rather than the event position to get
        // a cursor position, since the event position is the mouse position
        // rather than the text cursor position.
        //
        if (QContextMenuEvent::Keyboard == contextEvent->reason()) {
            d_func()->cursorForWord = this->textCursor();
        }
        // Else process as mouse event.
        //
        else {
            d_func()->cursorForWord = cursorForPosition(contextEvent->pos());
        }

        QTextCharFormat::UnderlineStyle spellingErrorUnderlineStyle =
            (QTextCharFormat::UnderlineStyle)
            QApplication::style()->styleHint
            (
                QStyle::SH_SpellCheckUnderlineStyle
            );

        // Get the formatting for the cursor position under the mouse,
        // and see if it has the spell check error underline style.
        //
        bool wordHasSpellingError = false;
        int blockPosition = d_func()->cursorForWord.positionInBlock();
#if (QT_VERSION_MAJOR == 5) && (QT_VERSION_MINOR < 6)
        QList<QTextLayout::FormatRange> formatList =
            cursorForWord.block().layout()->additionalFormats();
#else
        QVector<QTextLayout::FormatRange> formatList =
            d_func()->cursorForWord.block().layout()->formats();
#endif
        int mispelledWordStartPos = 0;
        int mispelledWordLength = 0;

        for (int i = 0; i < formatList.length(); i++) {
            QTextLayout::FormatRange formatRange = formatList[i];

            if
            (
                (blockPosition >= formatRange.start)
                && (blockPosition <= (formatRange.start + formatRange.length))
                && (formatRange.format.underlineStyle() == spellingErrorUnderlineStyle)
            ) {
                mispelledWordStartPos = formatRange.start;
                mispelledWordLength = formatRange.length;
                wordHasSpellingError = true;
                break;
            }
        }

        // The word under the mouse is spelled correctly, so use the default
        // processing for the context menu and return.
        //
        if (!wordHasSpellingError) {
            return QPlainTextEdit::eventFilter(watched, event);
        }

        // Select the misspelled word.
        d_func()->cursorForWord.movePosition
        (
            QTextCursor::PreviousCharacter,
            QTextCursor::MoveAnchor,
            blockPosition - mispelledWordStartPos
        );
        d_func()->cursorForWord.movePosition
        (
            QTextCursor::NextCharacter,
            QTextCursor::KeepAnchor,
            mispelledWordLength
        );

        d_func()->wordUnderMouse = d_func()->cursorForWord.selectedText();
        QStringList suggestions = d_func()->dictionary.suggestions(d_func()->wordUnderMouse);
        QMenu *popupMenu = createStandardContextMenu();
        QAction *firstAction = popupMenu->actions().first();

        d_func()->spellingActions.clear();

        if (!suggestions.empty()) {
            for (int i = 0; i < suggestions.size(); i++) {
                QAction *suggestionAction = new QAction(suggestions[i], this);

                // Need the following line because KDE Plasma 5 will insert a hidden ampersand
                // into the menu text as a keyboard accelerator.  Go off of the data in the
                // QAction rather than the text to avoid this.
                //
                suggestionAction->setData(suggestions[i]);

                d_func()->spellingActions.append(suggestionAction);
                popupMenu->insertAction(firstAction, suggestionAction);
            }
        } else {
            QAction *noSuggestionsAction =
                new QAction(tr("No spelling suggestions found"), this);
            noSuggestionsAction->setEnabled(false);
            d_func()->spellingActions.append(noSuggestionsAction);
            popupMenu->insertAction(firstAction, noSuggestionsAction);
        }

        popupMenu->insertSeparator(firstAction);
        popupMenu->insertAction(firstAction, d_func()->addWordToDictionaryAction);
        popupMenu->insertSeparator(firstAction);
        popupMenu->insertAction(firstAction, d_func()->checkSpellingAction);
        popupMenu->insertSeparator(firstAction);

        // Show menu
        connect(popupMenu, SIGNAL(triggered(QAction *)), this, SLOT(suggestSpelling(QAction *)));

        QPoint menuPos;

        // If event was triggered by a key press, use the text cursor
        // coordinates to display the popup menu.
        //
        if (QContextMenuEvent::Keyboard == contextEvent->reason()) {
            QRect cr = this->cursorRect();
            menuPos.setX(cr.x());
            menuPos.setY(cr.y() + (cr.height() / 2));
            menuPos = viewport()->mapToGlobal(menuPos);
        }
        // Else use the mouse coordinates from the context menu event.
        //
        else {
            menuPos = viewport()->mapToGlobal(contextEvent->pos());
        }

        popupMenu->exec(menuPos);

        delete popupMenu;

        for (int i = 0; i < d_func()->spellingActions.size(); i++) {
            delete d_func()->spellingActions[i];
        }

        d_func()->spellingActions.clear();

        return true;
    }
}

void MarkdownEditor::wheelEvent(QWheelEvent *e)
{
    Qt::KeyboardModifiers modifier = e->modifiers();

#if QT_VERSION >= 0x050000
    int numDegrees = 0;

    QPoint angleDelta = e->angleDelta();

    if (!angleDelta.isNull()) {
        numDegrees = angleDelta.y();
    }
#else
    int numDegrees = e->delta();
#endif

    if ((Qt::ControlModifier == modifier) && (0 != numDegrees)) {
        int fontSize = this->font().pointSize();

        if (numDegrees > 0) {
            fontSize += 1;
        } else {
            fontSize -= 1;
        }

        // check for negative value
        if (fontSize <= 0) {
            fontSize = 1;
        }

        setFont(this->font().family(), fontSize);
        emit fontSizeChanged(fontSize);
    } else {
        QPlainTextEdit::wheelEvent(e);
    }
}

void MarkdownEditor::navigateDocument(const int position)
{
    QTextCursor cursor = this->textCursor();

    cursor.setPosition(position);
    this->setTextCursor(cursor);
    this->activateWindow();
    this->setFocus();
}

void MarkdownEditor::bold()
{
    d_func()->insertFormattingMarkup("**");
}

void MarkdownEditor::italic()
{
    d_func()->insertFormattingMarkup("*");
}

void MarkdownEditor::strikethrough()
{
    d_func()->insertFormattingMarkup("~~");
}

void MarkdownEditor::insertComment()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection()) {
        QString text = cursor.selectedText();
        text = QString("<!-- " + text + " -->");
        cursor.insertText(text);
    } else {
        cursor.insertText("<!--  -->");
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 4);
        this->setTextCursor(cursor);
    }
}

void MarkdownEditor::createBulletListWithAsteriskMarker()
{
    d_func()->insertPrefixForBlocks("* ");
}

void MarkdownEditor::createBulletListWithMinusMarker()
{
    d_func()->insertPrefixForBlocks("- ");
}

void MarkdownEditor::createBulletListWithPlusMarker()
{
    d_func()->insertPrefixForBlocks("+ ");
}

void MarkdownEditor::createNumberedListWithPeriodMarker()
{
    d_func()->createNumberedList('.');
}

void MarkdownEditor::createNumberedListWithParenthesisMarker()
{
    d_func()->createNumberedList(')');
}

void MarkdownEditor::createTaskList()
{
    d_func()->insertPrefixForBlocks("- [ ] ");
}

void MarkdownEditor::createBlockquote()
{
    d_func()->insertPrefixForBlocks("> ");
}

// Algorithm lifted from ReText.
void MarkdownEditor::removeBlockquote()
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = this->document()->findBlock(cursor.selectionStart());
        end = this->document()->findBlock(cursor.selectionEnd()).next();
    } else {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    while (block != end) {
        cursor.setPosition(block.position());

        if (this->document()->characterAt(cursor.position()) == '>') {
            cursor.deleteChar();

            // Delete first space that follow the '>' character, to clean up the
            // paragraph.
            //
            if (this->document()->characterAt(cursor.position()).isSpace()) {
                cursor.deleteChar();
            }
        }

        block = block.next();
    }

    cursor.endEditBlock();
}

// Algorithm lifted from ReText.
void MarkdownEditor::indentText()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection()) {
        QTextBlock block = this->document()->findBlock(cursor.selectionStart());
        QTextBlock end = this->document()->findBlock(cursor.selectionEnd()).next();

        cursor.beginEditBlock();

        while (block != end) {
            cursor.setPosition(block.position());

            if (d_func()->insertSpacesForTabs) {
                QString indentText = "";

                for (int i = 0; i < d_func()->tabWidth; i++) {
                    indentText += QString(" ");
                }

                cursor.insertText(indentText);
            } else {
                cursor.insertText("\t");
            }

            block = block.next();
        }

        cursor.endEditBlock();
    } else {
        int indent = d_func()->tabWidth;
        QString indentText = "";
        QRegularExpressionMatch match;

        cursor.beginEditBlock();

        switch (cursor.block().userState() & MarkdownStateMask) {
        case MarkdownStateNumberedList:
            match = d_func()->emptyNumberedListRegex.match(cursor.block().text());

            if (match.hasMatch()) {
                QStringList capture = match.capturedTexts();

                // Restart numbering for the nested list.
                if (capture.size() == 2) {
                    QRegularExpression numberRegex("\\d+");

                    cursor.movePosition(QTextCursor::StartOfBlock);
                    cursor.movePosition
                    (
                        QTextCursor::EndOfBlock,
                        QTextCursor::KeepAnchor
                    );

                    QString replacementText = cursor.selectedText();
                    replacementText =
                        replacementText.replace
                        (
                            numberRegex,
                            "1"
                        );

                    cursor.insertText(replacementText);
                    cursor.movePosition(QTextCursor::StartOfBlock);
                }
            }
            break;
        case MarkdownStateTaskList:
            if (d_func()->emptyTaskListRegex.match(cursor.block().text()).hasMatch()) {
                cursor.movePosition(QTextCursor::StartOfBlock);
            }
            break;
        case MarkdownStateBulletPointList: {
            if (d_func()->emptyBulletListRegex.match(cursor.block().text()).hasMatch()) {
                if (d_func()->bulletPointCyclingEnabled) {
                    QChar oldBulletPoint = cursor.block().text().trimmed().at(0);
                    QChar newBulletPoint = oldBulletPoint;
                    {
                        if (oldBulletPoint == '*') {
                            newBulletPoint = '-';
                        } else if (oldBulletPoint == '-') {
                            newBulletPoint = '+';
                        } else {
                            newBulletPoint = '*';
                        }
                    }

                    cursor.movePosition(QTextCursor::StartOfBlock);
                    cursor.movePosition
                    (
                        QTextCursor::EndOfBlock,
                        QTextCursor::KeepAnchor
                    );

                    QString replacementText = cursor.selectedText();
                    replacementText =
                        replacementText.replace
                        (
                            oldBulletPoint,
                            newBulletPoint
                        );
                    cursor.insertText(replacementText);
                }

                cursor.movePosition(QTextCursor::StartOfBlock);
            }

            break;
        }
        default:
            indent = d_func()->tabWidth - (cursor.positionInBlock() % d_func()->tabWidth);
            break;
        }

        if (d_func()->insertSpacesForTabs) {
            for (int i = 0; i < indent; i++) {
                indentText += QString(" ");
            }
        } else {
            indentText = "\t";
        }

        cursor.insertText(indentText);
        cursor.endEditBlock();
    }
}

// Algorithm lifted from ReText.
void MarkdownEditor::unindentText()
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = this->document()->findBlock(cursor.selectionStart());
        end = this->document()->findBlock(cursor.selectionEnd()).next();
    } else {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    while (block != end) {
        cursor.setPosition(block.position());

        if (this->document()->characterAt(cursor.position()) == '\t') {
            cursor.deleteChar();
        } else {
            int pos = 0;

            while
            (
                (this->document()->characterAt(cursor.position()) == ' ')
                && (pos < d_func()->tabWidth)
            ) {
                pos += 1;
                cursor.deleteChar();
            }
        }

        block = block.next();
    }

    if
    (
        (MarkdownStateBulletPointList == (cursor.block().userState() & MarkdownStateMask))
        && (d_func()->emptyBulletListRegex.match(cursor.block().text()).hasMatch())
        && d_func()->bulletPointCyclingEnabled
    ) {
        QChar oldBulletPoint = cursor.block().text().trimmed().at(0);
        QChar newBulletPoint;

        if (oldBulletPoint == '*') {
            newBulletPoint = '+';
        } else if (oldBulletPoint == '-') {
            newBulletPoint = '*';
        } else {
            newBulletPoint = '-';
        }

        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition
        (
            QTextCursor::EndOfBlock,
            QTextCursor::KeepAnchor
        );

        QString replacementText = cursor.selectedText();
        replacementText =
            replacementText.replace
            (
                oldBulletPoint,
                newBulletPoint
            );
        cursor.insertText(replacementText);
    }


    cursor.endEditBlock();
}

bool MarkdownEditor::toggleTaskComplete()
{
    QTextCursor cursor = textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = this->document()->findBlock(cursor.selectionStart());
        end = this->document()->findBlock(cursor.selectionEnd()).next();
    } else {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    while (block != end) {
        QRegularExpressionMatch match;

        if
        (
            (MarkdownStateTaskList == (block.userState() & MarkdownStateMask))
            && (block.text().indexOf(d_func()->taskListRegex, 0, &match) == 0)
        ) {
            QStringList capture = match.capturedTexts();

            if (capture.size() == 2) {
                QChar value = capture.at(1)[0];
                QChar replacement;
                int index = block.text().indexOf(" [");

                if (index >= 0) {
                    index += 2;
                }

                if (value == 'x') {
                    replacement = ' ';
                } else {
                    replacement = 'x';
                }

                cursor.setPosition(block.position());
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition
                (
                    QTextCursor::Right,
                    QTextCursor::MoveAnchor,
                    index
                );

                cursor.deleteChar();
                cursor.insertText(replacement);
            }
        }

        block = block.next();
    }

    cursor.endEditBlock();
    return true;
}

void MarkdownEditor::setEnableLargeHeadingSizes(bool enable)
{
    d_func()->highlighter->setEnableLargeHeadingSizes(enable);
}

void MarkdownEditor::setAutoMatchEnabled(bool enable)
{
    d_func()->autoMatchEnabled = enable;
}

void MarkdownEditor::setAutoMatchEnabled(const QChar openingCharacter, bool enabled)
{
    d_func()->autoMatchFilter.insert(openingCharacter, enabled);
}

void MarkdownEditor::setBulletPointCyclingEnabled(bool enable)
{
    d_func()->bulletPointCyclingEnabled = enable;
}

void MarkdownEditor::setUseUnderlineForEmphasis(bool enable)
{
    d_func()->highlighter->setUseUnderlineForEmphasis(enable);
}

void MarkdownEditor::setItalicizeBlockquotes(bool enable)
{
    d_func()->highlighter->setItalicizeBlockquotes(enable);
}

void MarkdownEditor::setInsertSpacesForTabs(bool enable)
{
    d_func()->insertSpacesForTabs = enable;
}

void MarkdownEditor::setTabulationWidth(int width)
{
    QFontMetrics fontMetrics(font());
    d_func()->tabWidth = width;

#if (QT_VERSION_MAJOR == 5) && (QT_VERSION_MINOR < 10)
    this->setTabStopWidth(fontMetrics.width(' ') * d_func()->tabWidth);
#else
    this->setTabStopDistance(fontMetrics.horizontalAdvance(' ') * d_func()->tabWidth);
#endif
}

void MarkdownEditor::setEditorWidth(EditorWidth width)
{
    d_func()->editorWidth = width;
}

void MarkdownEditor::setEditorCorners(InterfaceStyle corners)
{
    d_func()->editorCorners = corners;
}

void MarkdownEditor::runSpellChecker()
{
    if (d_func()->spellCheckEnabled) {
        SpellChecker::checkDocument(this, d_func()->highlighter, d_func()->dictionary);
    } else {
        SpellChecker::checkDocument(this, NULL, d_func()->dictionary);
    }
}

void MarkdownEditor::setSpellCheckEnabled(const bool enabled)
{
    d_func()->spellCheckEnabled = enabled;
    d_func()->highlighter->setSpellCheckEnabled(enabled);
}

void MarkdownEditor::increaseFontSize()
{
    int fontSize = this->font().pointSize() + 1;

    setFont(this->font().family(), fontSize);
    emit fontSizeChanged(fontSize);

}

void MarkdownEditor::decreaseFontSize()
{
    int fontSize = this->font().pointSize() - 1;

    // check for negative value
    if (fontSize <= 0) {
        fontSize = 1;
    }

    setFont(this->font().family(), fontSize);
    emit fontSizeChanged(fontSize);
}

void MarkdownEditor::suggestSpelling(QAction *action)
{
    if (action == d_func()->addWordToDictionaryAction) {
        this->setTextCursor(d_func()->cursorForWord);
        d_func()->dictionary.addToPersonal(d_func()->wordUnderMouse);
        d_func()->highlighter->rehighlight();
    } else if (action == d_func()->checkSpellingAction) {
        this->setTextCursor(d_func()->cursorForWord);
        SpellChecker::checkDocument(this, d_func()->highlighter, d_func()->dictionary);
    } else if (d_func()->spellingActions.contains(action)) {
        d_func()->cursorForWord.insertText(action->data().toString());
    }
}

void MarkdownEditor::onContentsChanged(int position, int charsAdded, int charsRemoved)
{
    Q_UNUSED(position)
    Q_UNUSED(charsAdded)
    Q_UNUSED(charsRemoved)

    d_func()->parseDocument();

    // Don't use the textChanged() or contentsChanged() (no parameters) signals
    // for checking if the typingResumed() signal needs to be emitted.  These
    // two signals are emitted even when the text formatting changes (i.e.,
    // when the QSyntaxHighlighter formats the text). Instead, use QTextDocument's
    // onContentsChanged(int, int, int) signal, which is only emitted when the
    // document text actually changes.
    //
    if (d_func()->typingHasPaused || d_func()->scaledTypingHasPaused) {
        d_func()->typingHasPaused = false;
        d_func()->scaledTypingHasPaused = false;
        d_func()->typingPausedSignalSent = false;
        d_func()->typingPausedScaledSignalSent = false;
        emit typingResumed();
    }
}

void MarkdownEditor::onTextBlockRemoved(const QTextBlock &)
{
    d_func()->parseDocument();
}

void MarkdownEditor::onSelectionChanged()
{
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection()) {
        emit textSelected
        (
            cursor.selectedText(),
            cursor.selectionStart(),
            cursor.selectionEnd()
        );
    } else {
        emit textDeselected();
    }
}

void MarkdownEditor::focusText()
{
    if (FocusModeDisabled != d_func()->focusMode) {
        QTextEdit::ExtraSelection beforeFadedSelection;
        QTextEdit::ExtraSelection afterFadedSelection;
        beforeFadedSelection.format.setForeground(d_func()->fadeColor);
        beforeFadedSelection.cursor = this->textCursor();
        afterFadedSelection.format.setForeground(d_func()->fadeColor);
        afterFadedSelection.cursor = this->textCursor();

        bool canFadePrevious = false;

        QList<QTextEdit::ExtraSelection> selections;

        switch (d_func()->focusMode) {
        case FocusModeCurrentLine: // Current line
            beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfLine);
            canFadePrevious = beforeFadedSelection.cursor.movePosition(QTextCursor::Up);
            beforeFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
            beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

            if (canFadePrevious) {
                selections.append(beforeFadedSelection);
            }

            afterFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
            afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            selections.append(afterFadedSelection);
            break;

        case FocusModeThreeLines: // Current line and previous two lines
            beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfLine);
            canFadePrevious = beforeFadedSelection.cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 2);
            beforeFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
            beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

            if (canFadePrevious) {
                selections.append(beforeFadedSelection);
            }

            afterFadedSelection.cursor.movePosition(QTextCursor::Down);
            afterFadedSelection.cursor.movePosition(QTextCursor::EndOfLine);
            afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            selections.append(afterFadedSelection);
            break;

        case FocusModeParagraph: // Current paragraph
            canFadePrevious = beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfBlock);
            beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
            selections.append(beforeFadedSelection);
            afterFadedSelection.cursor.movePosition(QTextCursor::EndOfBlock);
            afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            selections.append(afterFadedSelection);
            break;

        case FocusModeSentence: { // Current sentence
            QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Sentence, this->textCursor().block().text());
            int currentPos = this->textCursor().positionInBlock();
            int lastSentencePos = 0;
            int nextSentencePos = 0;

            boundaryFinder.setPosition(currentPos);
            lastSentencePos = boundaryFinder.toPreviousBoundary();
            boundaryFinder.setPosition(currentPos);
            nextSentencePos = boundaryFinder.toNextBoundary();

            if (lastSentencePos < 0) {
                beforeFadedSelection.cursor.movePosition(QTextCursor::StartOfBlock);
            } else {
                beforeFadedSelection.cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, currentPos - lastSentencePos);
            }

            beforeFadedSelection.cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
            selections.append(beforeFadedSelection);

            if (nextSentencePos < 0) {
                afterFadedSelection.cursor.movePosition(QTextCursor::EndOfBlock);
            } else {
                afterFadedSelection.cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nextSentencePos - currentPos);
            }

            afterFadedSelection.cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            selections.append(afterFadedSelection);

            break;
        }
        // `FocusModeTypewriter` implicitly handeled here as we don't highlight anything but center the current line.
        default:
            break;
        }

        this->setExtraSelections(selections);
    }
}

void MarkdownEditor::checkIfTypingPaused()
{
    if (d_func()->typingHasPaused && !d_func()->typingPausedSignalSent) {
        d_func()->typingPausedSignalSent = true;
        emit typingPaused();
    }

    d_func()->typingTimer->stop();
    d_func()->typingTimer->start(1000);

    d_func()->typingHasPaused = true;
}

void MarkdownEditor::checkIfTypingPausedScaled()
{
    if (d_func()->scaledTypingHasPaused && !d_func()->typingPausedScaledSignalSent) {
        d_func()->typingPausedScaledSignalSent = true;
        emit typingPausedScaled();
    }

    // Scale timer interval based on document size.
    int interval = (document()->characterCount() / 30000) * 20;

    if (interval > 1000) {
        interval = 1000;
    } else if (interval < 20) {
        interval = 20;
    }

    d_func()->scaledTypingTimer->stop();
    d_func()->scaledTypingTimer->start(interval);

    d_func()->scaledTypingHasPaused = true;
}

void MarkdownEditor::spellCheckFinished(int result)
{
    Q_UNUSED(result)

    d_func()->highlighter->rehighlight();
}

void MarkdownEditor::onCursorPositionChanged()
{
    if (!d_func()->mouseButtonDown) {
        QRect cursor = this->cursorRect();
        QRect viewport = this->viewport()->rect();
        int bottom = viewport.bottom() - this->fontMetrics().height();

        if
        (
            (d_func()->focusMode != FocusModeDisabled) ||
            (cursor.bottom() >= bottom) ||
            (cursor.top() <= viewport.top())
        ) {
            centerCursor();
        }
    }

    // Set the text cursor back to visible and reset the blink timer so that
    // the cursor is always visible whenever it moves to a new position.
    //
    d_func()->textCursorVisible = true;
    d_func()->cursorBlinkTimer->stop();
    d_func()->cursorBlinkTimer->start();

    // Update widget to ensure cursor is drawn.
    update();

    emit cursorPositionChanged(this->textCursor().position());
}

void MarkdownEditorPrivate::toggleCursorBlink()
{
    this->textCursorVisible = !this->textCursorVisible;
    q_func()->update();
}

void MarkdownEditorPrivate::parseDocument()
{
    MarkdownAST *ast =
        CmarkGfmAPI::instance()->parse
        (
            q_func()->document()->toPlainText(),
            false
        );

    // Note:  MarkdownDocument is responsible for freeing memory
    // allocated for the AST.
    //
    ((MarkdownDocument *) q_func()->document())->setMarkdownAST(ast);
}

void MarkdownEditorPrivate::handleCarriageReturn()
{
    QString autoInsertText = "";
    QTextCursor cursor = q_func()->textCursor();
    bool endList = false;

    if (cursor.positionInBlock() < (cursor.block().length() - 1)) {
        autoInsertText = priorIndentation();

        if (cursor.positionInBlock() < autoInsertText.length()) {
            autoInsertText.truncate(cursor.positionInBlock());
        }
    } else {
        QRegularExpressionMatch match;

        switch (cursor.block().userState() & MarkdownStateMask) {
        case MarkdownStateNumberedList: {
            autoInsertText = priorMarkdownBlockItemStart(numberedListRegex, match);
            QStringList capture = match.capturedTexts();

            if (!autoInsertText.isEmpty() && (capture.size() == 2)) {
                // If the line of text is an empty list item, end the list.
                if (cursor.block().text().length() == autoInsertText.length()) {
                    endList = true;
                }
                // Else auto-increment the list number.
                else {
                    QRegularExpression numberRegex("\\d+");
                    int number = capture.at(1).toInt();
                    number++;
                    autoInsertText =
                        autoInsertText.replace
                        (
                            numberRegex,
                            QString("%1").arg(number)
                        );
                }
            } else {
                autoInsertText = priorIndentation();
            }
            break;
        }
        case MarkdownStateTaskList:
            autoInsertText = priorMarkdownBlockItemStart(taskListRegex, match);
            // If the line of text is an empty list item, end the list.
            if (cursor.block().text().length() == autoInsertText.length()) {
                endList = true;
            } else {
                // In case the previous line had a completed task with
                // an X checking it off, make sure a completed task
                // isn't added as the new task (remove the x and replace
                // with a space).
                //
                autoInsertText = autoInsertText.replace('x', ' ');
            }
            break;
        case MarkdownStateBulletPointList:
            autoInsertText = priorMarkdownBlockItemStart(bulletListRegex, match);

            if (autoInsertText.isEmpty()) {
                autoInsertText = priorIndentation();
            }
            // If the line of text is an empty list item, end the list.
            else if (cursor.block().text().length() == autoInsertText.length()) {
                endList = true;
            }

            break;
        case MarkdownStateBlockquote:
            autoInsertText = priorMarkdownBlockItemStart(blockquoteRegex, match);
            break;
        default:
            autoInsertText = priorIndentation();
            break;
        }
    }

    if (endList) {
        autoInsertText = priorIndentation();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.insertText(autoInsertText);
        autoInsertText = "";
    }

    cursor.insertText(QString("\n") + autoInsertText);
    q_func()->ensureCursorVisible();
}

bool MarkdownEditorPrivate::handleBackspaceKey()
{
    QTextCursor cursor = q_func()->textCursor();

    if (cursor.hasSelection()) {
        return false;
    }

    int backtrackIndex = -1;

    switch (cursor.block().userState() & MarkdownStateMask) {
    case MarkdownStateNumberedList: {
        if (emptyNumberedListRegex.match(q_func()->textCursor().block().text()).hasMatch()) {
            backtrackIndex = cursor.block().text().indexOf(QRegularExpression("\\d"));
        }
        break;
    }
    case MarkdownStateTaskList:
        if
        (
            emptyBulletListRegex.match(cursor.block().text()).hasMatch()
            || emptyTaskListRegex.match(cursor.block().text()).hasMatch()
        ) {
            backtrackIndex = cursor.block().text().indexOf(QRegularExpression("[+*-]"));
        }
        break;
    case MarkdownStateBlockquote:
        if (emptyBlockquoteRegex.match(cursor.block().text()).hasMatch()) {
            backtrackIndex = cursor.block().text().lastIndexOf('>');
        }
        break;
    default:
        // If the first character in an automatched set is being
        // deleted, then delete the second matching one along with it.
        //
        if (autoMatchEnabled && (cursor.positionInBlock() > 0)) {
            QString blockText = cursor.block().text();

            if (cursor.positionInBlock() < blockText.length()) {
                QChar currentChar = blockText[cursor.positionInBlock()];
                QChar previousChar = blockText[cursor.positionInBlock() - 1];

                if (markupPairs.value(previousChar) == currentChar) {
                    cursor.movePosition(QTextCursor::Left);
                    cursor.movePosition
                    (
                        QTextCursor::Right,
                        QTextCursor::KeepAnchor,
                        2
                    );
                    cursor.removeSelectedText();
                    return true;
                }
            }
        }
        break;
    }

    if (backtrackIndex >= 0) {
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition
        (
            QTextCursor::Right,
            QTextCursor::MoveAnchor,
            backtrackIndex
        );

        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        return true;
    }

    return false;
}

// Algorithm lifted from ReText.
void MarkdownEditorPrivate::insertPrefixForBlocks(const QString &prefix)
{
    QTextCursor cursor = q_func()->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = q_func()->document()->findBlock(cursor.selectionStart());
        end = q_func()->document()->findBlock(cursor.selectionEnd()).next();
    } else {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    while (block != end) {
        cursor.setPosition(block.position());
        cursor.insertText(prefix);
        block = block.next();
    }

    cursor.endEditBlock();
}

void MarkdownEditorPrivate::createNumberedList(const QChar marker)
{
    QTextCursor cursor = q_func()->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = q_func()->document()->findBlock(cursor.selectionStart());
        end = q_func()->document()->findBlock(cursor.selectionEnd()).next();
    } else {
        block = cursor.block();
        end = block.next();
    }

    cursor.beginEditBlock();

    int number = 1;

    while (block != end) {
        cursor.setPosition(block.position());
        cursor.insertText(QString("%1").arg(number) + marker + " ");
        block = block.next();
        number++;
    }

    cursor.endEditBlock();
}

bool MarkdownEditorPrivate::insertPairedCharacters(const QChar firstChar)
{
    if
    (
        autoMatchEnabled
        && markupPairs.contains(firstChar)
        && autoMatchFilter.value(firstChar)
    ) {
        QChar lastChar = markupPairs.value(firstChar);
        QTextCursor cursor = q_func()->textCursor();
        QTextBlock block;
        QTextBlock end;

        if (cursor.hasSelection()) {
            block = q_func()->document()->findBlock(cursor.selectionStart());
            end = q_func()->document()->findBlock(cursor.selectionEnd());

            // Only surround selection with matched characters if the
            // selection belongs to the same block.
            //
            if (block == end) {
                cursor.beginEditBlock();
                cursor.setPosition(cursor.selectionStart());
                cursor.insertText(firstChar);
                cursor.setPosition(q_func()->textCursor().selectionEnd());
                cursor.insertText(lastChar);
                cursor.endEditBlock();

                cursor = q_func()->textCursor();
                cursor.setPosition(cursor.selectionStart());
                cursor.setPosition
                (
                    q_func()->textCursor().selectionEnd() - 1,
                    QTextCursor::KeepAnchor
                );
                q_func()->setTextCursor(cursor);
                return true;
            }
        } else {
            // Get the previous character.  Ensure that it is whitespace.
            int blockPos = cursor.positionInBlock();
            bool doMatch = true;

            // If not at the beginning of the line...
            if (blockPos > 0) {
                blockPos--;

                if (!cursor.block().text()[blockPos].isSpace()) {
                    // If the previous character is not whitespace, allow
                    // character matching only for parentheses and similar
                    // characters that need matching even if preceeded by
                    // non-whitespace (i.e., for mathematical or computer
                    // science expressions).  Otherwise, do not match the
                    // opening character.
                    //
                    switch (firstChar.toLatin1()) {
                    case '(':
                    case '[':
                    case '{':
                    case '<':
                        break;
                    default:
                        doMatch = false;
                        break;
                    }
                }
            }

            // If we are not at the end of the block
            // and the next character is not whitespace,
            // most likely the user is either manually adding a pair of
            // characters around some text,
            // or there's no need for a closing character at all.
            if (!cursor.atBlockEnd() && !cursor.block().text()[blockPos + 1].isSpace()) {
                doMatch = false;
            }

            if (doMatch) {
                cursor.insertText(firstChar);
                cursor.insertText(lastChar);
                cursor.movePosition(QTextCursor::PreviousCharacter);
                q_func()->setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

bool MarkdownEditorPrivate::handleEndPairCharacterTyped(const QChar ch)
{
    QTextCursor cursor = q_func()->textCursor();

    bool lookAhead = false;

    if (this->autoMatchEnabled && !cursor.hasSelection()) {
        QList<QChar> values = this->markupPairs.values();

        if (values.contains(ch)) {
            QChar key = this->markupPairs.key(ch);

            if (this->autoMatchFilter.value(key)) {
                lookAhead = true;
            }
        }
    }

    if (lookAhead) {
        QTextCursor cursor = q_func()->textCursor();
        QString text = cursor.block().text();
        int pos = cursor.positionInBlock();

        if (pos < (text.length())) {
            // Look ahead to the character after the cursor position. If it
            // matches the character that was entered, then move the cursor
            // one position forward.
            //
            if (text[pos] == ch) {
                cursor.movePosition(QTextCursor::NextCharacter);
                q_func()->setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

bool MarkdownEditorPrivate::handleWhitespaceInEmptyMatch(const QChar whitespace)
{
    QTextCursor cursor = q_func()->textCursor();
    QTextBlock block = cursor.block();
    QString text = block.text();
    int pos = cursor.positionInBlock();

    if
    (
        (text.length() > 0) &&
        (pos > 0) &&
        (pos < text.length()) &&
        this->nonEmptyMarkupPairs.contains(text[pos - 1]) &&
        (text[pos] == this->nonEmptyMarkupPairs.value(text[pos - 1]))
    ) {
        cursor.deleteChar();
        cursor.insertText(whitespace);
        return true;
    }

    return false;
}

void MarkdownEditorPrivate::insertFormattingMarkup(const QString &markup)
{
    QTextCursor cursor = q_func()->textCursor();

    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd() + markup.length();
        QTextCursor c = cursor;
        c.beginEditBlock();
        c.setPosition(start);
        c.insertText(markup);
        c.setPosition(end);
        c.insertText(markup);
        c.endEditBlock();
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::QTextCursor::KeepAnchor, markup.length());
        q_func()->setTextCursor(cursor);
    } else {
        // Insert markup twice (for opening and closing around the cursor),
        // and then move the cursor to be between the pair.
        //
        cursor.beginEditBlock();
        cursor.insertText(markup);
        cursor.insertText(markup);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, markup.length());
        cursor.endEditBlock();
        q_func()->setTextCursor(cursor);
    }
}

QString MarkdownEditorPrivate::priorIndentation()
{
    QString indent = "";
    QTextCursor cursor = q_func()->textCursor();
    QTextBlock block = cursor.block();

    QString text = block.text();

    for (int i = 0; i < text.length(); i++) {
        if (text[i].isSpace()) {
            indent += text[i];
        } else {
            return indent;
        }
    }

    return indent;
}

QString MarkdownEditorPrivate::priorMarkdownBlockItemStart
(
    const QRegularExpression &itemRegex,
    QRegularExpressionMatch &match
)
{
    QTextCursor cursor = q_func()->textCursor();
    QTextBlock block = cursor.block();

    QString text = block.text();

    if ((text.indexOf(itemRegex, 0, &match) >= 0) && match.hasMatch()) {
        return match.captured();
    }

    return QString("");
}

bool MarkdownEditorPrivate::atBlockAreaStart(const QTextBlock &block, BlockType &type) const
{
    if (!block.isValid()) {
        type = BlockTypeNone;
        return false;
    }

    if (this->atCodeBlockStart(block)) {
        type = BlockTypeCode;
        return true;
    }

    if (this->isBlockquote(block)) {
        type = BlockTypeQuote;
        return true;
    }

    type = BlockTypeNone;
    return false;
}

bool MarkdownEditorPrivate::atBlockAreaEnd(const QTextBlock &block, const BlockType type) const
{
    switch (type) {
    case BlockTypeCode:
        return this->atCodeBlockEnd(block) && !this->isBlockquote(block);
    case BlockTypeQuote:
        return !this->isBlockquote(block);
    default:
        return true;
    }
}

bool MarkdownEditorPrivate::atCodeBlockStart(const QTextBlock &block) const
{
    return
        (
            (MarkdownStateCodeBlock == (MarkdownStateCodeBlock & block.userState()))
            && (MarkdownStateCodeBlock != (MarkdownStateCodeBlock & block.previous().userState()))
        );
}

bool MarkdownEditorPrivate::atCodeBlockEnd(const QTextBlock &block) const
{
    return (MarkdownStateCodeBlock != (MarkdownStateCodeBlock & block.userState()));
}

bool MarkdownEditorPrivate::isBlockquote(const QTextBlock &block) const
{
    return (MarkdownStateBlockquote == (MarkdownStateBlockquote & block.userState()));
}
} // namespace ghostwriter
