/*
 * SPDX-FileCopyrightText: 2014-2024 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>
 * SPDX-FileCopyrightText: 2012 Dmitry Shachnev
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>
#include <math.h>

#include <QApplication>
#include <QChar>
#include <QColor>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontMetricsF>
#include <QGridLayout>
#include <QImageReader>
#include <QImageWriter>
#include <QLayout>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QSize>
#include <QStandardPaths>
#include <QString>
#include <QStringLiteral>
#include <QTextBoundaryFinder>
#include <QTextCursor>
#include <QTimer>
#include <QUrl>

#include "../markdown/cmarkgfmapi.h"

#include "actions/inlinemarkuptoggle.h"
#include "markdowneditor.h"
#include "markdownhighlighter.h"
#include "markdownstates.h"

namespace ghostwriter
{

namespace
{
constexpr auto unbreakableSpace{" "}; // Entity: &nbsp; HTML code: &#160; Unicode: U+00AO
constexpr auto doubleSpace{"  "};
constexpr auto unbreakableSpaceIndicator{"_"};
}
class MarkdownEditorPrivate
{
    Q_DECLARE_PUBLIC(MarkdownEditor)

public:
    MarkdownEditorPrivate(MarkdownEditor *q_ptr)
        : q_ptr(q_ptr)
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

    static const int CursorWidth = 2;
    const QString lineBreakChar = QString::fromUtf8("↵");

    // We use only image MIME types that are web-friendly so that any inserted
    // or pasted images can be displayed in the live preview.
    static const QStringList webMimeTypes;

    static QStringList imageReadFormats;
    static QStringList imageWriteFormats;
    static QString imageOpenFilter;
    static QString imageSaveFilter;

    MarkdownEditor *q_ptr;

    MarkdownDocument *textDocument;
    MarkdownHighlighter *highlighter;
    QGridLayout *preferredLayout;
    bool autoMatchEnabled;
    bool bulletPointCyclingEnabled;
    bool hemingwayModeEnabled;
    bool showUnbreakableSpaces;
    FocusMode focusMode;
    QBrush fadeColor;
    QColor blockColor;
    QColor whitespaceRenderColor;
    QColor unbreakableSpaceRenderColor;
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
    // typingPausedScaled() signals: multiple times after they have
    // already been sent the first time after a pause in the user's
    // typing.
    //
    bool typingPausedSignalSent;
    bool typingPausedScaledSignalSent;

    // Flag to track when a document change is due to a setPlainText() call
    // versus the user editing the document.
    bool loadingDocument;

    void toggleCursorBlink();
    void parseDocument();
    void parseText(const QString &text);

    void handleCarriageReturn();
    bool handleBackspaceKey();
    void insertPrefixForBlocks(const QString &prefix);
    void createNumberedList(const QChar marker);
    bool insertPairedCharacters(const QChar firstChar);
    bool handleEndPairCharacterTyped(const QChar ch);
    bool handleWhitespaceInEmptyMatch(const QChar whitespace);
    QString priorIndentation();
    QString priorMarkdownBlockItemStart(
        const QRegularExpression &itemRegex,
        QRegularExpressionMatch &match
    );

    bool insideBlockArea(const QTextBlock &block, BlockType &type) const;
    bool atBlockAreaStart(const QTextBlock &block, BlockType &type) const;
    bool atBlockAreaEnd(const QTextBlock &block, const BlockType type) const;
    bool atCodeBlockStart(const QTextBlock &block) const;
    bool atCodeBlockEnd(const QTextBlock &block) const;
    bool isBlockquote(const QTextBlock &block) const;
    bool isCodeBlock(const QTextBlock &block) const;
    
    static QStringList buildImageReaderFormats();
    static QStringList buildImageWriterFormats();
    static QString buildImageFilters(
        const QStringList &mimeTypes,
        bool includeWildcardImages = false);
};

const QStringList MarkdownEditorPrivate::webMimeTypes = QStringList({
    QStringLiteral("image/png"),
    QStringLiteral("image/jpeg"),
    QStringLiteral("image/webp"),
    QStringLiteral("image/apng"),
    QStringLiteral("image/avif"),
    QStringLiteral("image/gif"),
    QStringLiteral("image/svg")
});

QStringList MarkdownEditorPrivate::imageReadFormats =
    MarkdownEditorPrivate::buildImageReaderFormats();

QStringList MarkdownEditorPrivate::imageWriteFormats =
    MarkdownEditorPrivate::buildImageWriterFormats();

QString MarkdownEditorPrivate::imageOpenFilter =
    MarkdownEditorPrivate::buildImageFilters(
        MarkdownEditorPrivate::imageReadFormats, true);

QString MarkdownEditorPrivate::imageSaveFilter =
    MarkdownEditorPrivate::buildImageFilters(
        MarkdownEditorPrivate::imageWriteFormats);

MarkdownEditor::MarkdownEditor
(
    MarkdownDocument *textDocument,
    const ColorScheme &colors,
    QWidget *parent
)
    : QPlainTextEdit(parent),
      d_ptr(new MarkdownEditorPrivate(this))
{
    Q_D(MarkdownEditor);
    
    d->textDocument = textDocument;
    d->autoMatchEnabled = true;
    d->bulletPointCyclingEnabled = true;
    d->mouseButtonDown = false;
    d->loadingDocument = false;

    this->setDocument(textDocument);
    this->setAcceptDrops(true);

    d->preferredLayout = new QGridLayout();
    d->preferredLayout->setSpacing(0);
    d->preferredLayout->setContentsMargins(0, 0, 0, 0);
    d->preferredLayout->addWidget(this, 0, 0);

    d->blockquoteRegex.setPattern("^ {0,3}(>\\s*)+");
    d->numberedListRegex.setPattern("^\\s*([0-9]+)[.)]\\s+");
    d->bulletListRegex.setPattern("^\\s*[+*-]\\s+");
    d->taskListRegex.setPattern("^\\s*[-*+] \\[([x ])\\]\\s+");
    d->emptyBlockquoteRegex.setPattern("^ {0,3}(>\\s*)+$");
    d->emptyNumberedListRegex.setPattern("^\\s*([0-9]+)[.)]\\s+$");
    d->emptyBulletListRegex.setPattern("^\\s*[+*-]\\s+$");
    d->emptyTaskListRegex.setPattern("^\\s*[-*+] \\[([x ])\\]\\s+$");

    this->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setShowTabsAndSpacesEnabled(false);
    this->setShowUnbreakableSpaces(false);

    // Make sure QPlainTextEdit does not draw a cursor.  (We'll paint it manually.)
    this->setCursorWidth(0);

    this->setCenterOnScroll(true);
    this->ensureCursorVisible();
    d->hemingwayModeEnabled = false;
    d->focusMode = FocusModeDisabled;
    d->insertSpacesForTabs = false;
    this->setTabulationWidth(4);
    d->editorWidth = EditorWidthMedium;
    d->editorCorners = InterfaceStyleRounded;

    d->markupPairs.insert('"', '"');
    d->markupPairs.insert('\'', '\'');
    d->markupPairs.insert('(', ')');
    d->markupPairs.insert('[', ']');
    d->markupPairs.insert('{', '}');
    d->markupPairs.insert('*', '*');
    d->markupPairs.insert('_', '_');
    d->markupPairs.insert('`', '`');
    d->markupPairs.insert('<', '>');

    // Set automatching for the above markup pairs to be
    // enabled by default.
    //
    d->autoMatchFilter.insert('"', true);
    d->autoMatchFilter.insert('\'', true);
    d->autoMatchFilter.insert('(', true);
    d->autoMatchFilter.insert('[', true);
    d->autoMatchFilter.insert('{', true);
    d->autoMatchFilter.insert('*', true);
    d->autoMatchFilter.insert('_', true);
    d->autoMatchFilter.insert('`', true);
    d->autoMatchFilter.insert('<', true);

    d->nonEmptyMarkupPairs.insert('*', '*');
    d->nonEmptyMarkupPairs.insert('_', '_');
    d->nonEmptyMarkupPairs.insert('<', '>');

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(this->document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(onContentsChanged(int, int, int)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

    d->highlighter = new MarkdownHighlighter(this, colors);

    d->typingPausedSignalSent = true;
    d->typingHasPaused = true;

    d->typingTimer = new QTimer(this);
    connect
    (
        d->typingTimer,
        SIGNAL(timeout()),
        this,
        SLOT(checkIfTypingPaused())
    );
    d->typingTimer->start(1000);

    d->typingPausedScaledSignalSent = true;
    d->scaledTypingHasPaused = true;

    d->scaledTypingTimer = new QTimer(this);
    connect
    (
        d->scaledTypingTimer,
        SIGNAL(timeout()),
        this,
        SLOT(checkIfTypingPausedScaled())
    );
    d->scaledTypingTimer->start(1000);

    this->setColorScheme(colors);
    d->textCursorVisible = true;

    d->cursorBlinkTimer = new QTimer(this);
    this->connect
    (
        d->cursorBlinkTimer,
        &QTimer::timeout,
        [d]() {
            d->toggleCursorBlink();
        }
    );
    d->cursorBlinkTimer->start(500);
}

MarkdownEditor::~MarkdownEditor()
{
    ;
}


QSyntaxHighlighter *MarkdownEditor::highlighter() const
{
    Q_D(const MarkdownEditor);
    return d->highlighter;
}

void MarkdownEditor::paintEvent(QPaintEvent *event)
{
    Q_D(MarkdownEditor);
    
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

    if (InterfaceStyleSquare == d->editorCorners) {
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
        MarkdownEditorPrivate::BlockType prevType;
        
        QRectF r = this->blockBoundingRect(block).translated(offset);

        // If the first visible block is in the middle of a text block area...
        if (firstVisible 
                && d->insideBlockArea(block, blockType)
                && d->insideBlockArea(block.previous(), prevType)
                && (blockType == prevType)) {
            clipTop = true;
            inBlockArea = true;
            blockAreaRect = r;
            dy = 0;
        }
        // If the block begins a new text block area...
        else if (!inBlockArea && d->atBlockAreaStart(block, blockType)) {
            blockAreaRect = r;
            dy = 0;
            inBlockArea = true;

            // If this is the first visible block within the viewport
            // and if the previous block is part of the text block area,
            // then the rectangle to draw for the block area will have
            // its top clipped by the viewport and will need to be
            // drawn specially.
            //
            if
            (
                firstVisible
                && d->insideBlockArea(block.previous(), prevType)
                && (blockType == prevType)
            ) {
                clipTop = true;
            }
        }
        // Else if the block ends a text block area...
        else if (inBlockArea && d->atBlockAreaEnd(block, blockType)) {
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
            painter.setBrush(QBrush(d->blockColor));

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

    // Find all Markdown line breaks, and draw line break symbols for them.
    block = firstVisibleBlock();
    done = false;
    offset = contentOffset();

    while (block.isValid() && !done) {        
        QRectF r = this->blockBoundingRect(block).translated(offset);
        auto text = block.text();

        // If not in a code block, and the current block ends with two spaces
        // to indicate a line break in Markdown syntax, then draw the line
        // break symbol at the end of the block.
        // 
        // Also only draw the line break symbol if the text cursor is not at
        // the end of the line, since it's rather annoying when typing in the
        // at the end of a sentence and is in the habit of typing double spaces
        // after punctuation.
        //
        if (!d->isCodeBlock(block)
                && text.endsWith(doubleSpace) 
                && (this->textCursor().position()
                    != (block.position() + block.length() - 1))) {
            // Get position of last space character in the block.
            QTextCursor c(block);
            c.movePosition(QTextCursor::EndOfBlock);
            c.movePosition(QTextCursor::PreviousCharacter);
            QRect spaceRect = this->cursorRect(c);

            // Calculate where to draw the line break character. We want it
            // drawn over the space character.
            QFontMetrics m(this->font());
            QRect breakRect = m.tightBoundingRect(d->lineBreakChar);
            dy = (breakRect.height() - spaceRect.height()) / 2;
            QPoint pos(spaceRect.left(), spaceRect.bottom() + dy);

            // Draw the line break character!
            QPainter painter(viewport());
            painter.setFont(this->font());

            if (!this->textCursor().hasSelection()
                    || (c.position() >= this->textCursor().selectionEnd())
                    || (c.position() < this->textCursor().selectionStart())) {
                painter.setPen(d->whitespaceRenderColor);
            }

            painter.drawText(pos, d->lineBreakChar);
            painter.end();
        }
        if (d->showUnbreakableSpaces 
                && text.contains(unbreakableSpace)) {
            // unbreakable space
            QTextCursor c(block);
            QRect blockRect = this->cursorRect(c);
            QFontMetrics m(c.charFormat().font());

            auto h = (MarkdownStateMask & block.userState());
            auto it =  std::find_if(std::begin(MarkdownHeaderStates), std::end(MarkdownHeaderStates), [h](const MarkdownState& state){
                return state == h;
            });
            if(it != std::end(MarkdownHeaderStates)) {
                auto f = c.charFormat().font();
                f.setPointSize(f.pointSize() + std::distance(it, std::end(MarkdownHeaderStates)));
                m = QFontMetrics(f);
            }

            QRect indicatorRect = m.tightBoundingRect(unbreakableSpaceIndicator);

            int i = 0;
            QPainter painter(viewport());
            painter.setFont(c.charFormat().font());
            painter.setPen(d->unbreakableSpaceRenderColor);

            do {
                int idx = text.indexOf(unbreakableSpace, i);
                QPoint pos(blockRect.x() + m.horizontalAdvance(text, idx), blockRect.bottom() - indicatorRect.height());

                if (idx < 0) {
                    i = text.size(); // finished
                } else {
                    painter.drawText(pos, unbreakableSpaceIndicator);
                    i = idx + 1;
                }

            } while (i < text.size());
            painter.end();
        }

        block = block.next();
        offset.ry() += r.height();

        // If this is the last text block visible within the viewport...
        if (offset.y() > viewportRect.height()) {
            // Finished drawing.
            done = true;
        }
    }

    // Draw the text cursor/caret.
    if (d->textCursorVisible && this->hasFocus()) {
        // Get the cursor rect so that we have the ideal height for it,
        // and then set it to be 2 pixels wide.  (The width will be zero,
        // because we set it to be that in the constructor so that
        // QPlainTextEdit will not draw another cursor underneath this one.)
        //
        QPainter painter(viewport());
        painter.fillRect(cursorRect(), QBrush(d->cursorColor));
        painter.end();
    }
}

void MarkdownEditor::setPlainText(const QString &text)
{
    Q_D(MarkdownEditor);

    d->parseText(text);
    d->loadingDocument = true;
    d->typingHasPaused = true;
    d->scaledTypingHasPaused = true;
    QPlainTextEdit::setPlainText(text);
    d->loadingDocument = false;
}

QLayout *MarkdownEditor::preferredLayout()
{
    Q_D(MarkdownEditor);
    
    return d->preferredLayout;
}

bool MarkdownEditor::hemingwayModeEnabled() const
{
    Q_D(const MarkdownEditor);
    
    return d->hemingwayModeEnabled;
}

/**
 * Sets whether Hemingway mode is enabled.
 */
void MarkdownEditor::setHemingWayModeEnabled(bool enabled)
{
    Q_D(MarkdownEditor);
    
    d->hemingwayModeEnabled = enabled;
}

FocusMode MarkdownEditor::focusMode() const
{
    Q_D(const MarkdownEditor);
    
    return d->focusMode;
}

void MarkdownEditor::setFocusMode(FocusMode mode)
{
    Q_D(MarkdownEditor);
    
    d->focusMode = mode;

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
    Q_D(MarkdownEditor);
    
    d->highlighter->setColorScheme(colors);
    d->cursorColor = colors.cursor;
    d->whitespaceRenderColor = colors.listMarkup;
    d->unbreakableSpaceRenderColor = colors.error;
    d->blockColor = colors.foreground;
    d->blockColor.setAlpha(10);

    QColor fadedForegroundColor = colors.foreground;
    fadedForegroundColor.setAlpha(100);

    d->fadeColor = QBrush(fadedForegroundColor);
    this->focusText();
}

void MarkdownEditor::setFont(const QString &family, double pointSize)
{
    Q_D(MarkdownEditor);
    
    QFont font(family, pointSize);
    QPlainTextEdit::setFont(font);
    d->highlighter->setFont(family, pointSize);
    setTabulationWidth(d->tabWidth);
}

void MarkdownEditor::setShowTabsAndSpacesEnabled(bool enabled)
{
    Q_D(MarkdownEditor);
    
    QTextOption option = d->textDocument->defaultTextOption();

    if (enabled) {
        option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
    } else {
        option.setFlags(option.flags() & ~QTextOption::ShowTabsAndSpaces);
    }

    d->textDocument->setDefaultTextOption(option);
}

void MarkdownEditor::setShowUnbreakableSpaces(bool enabled)
{
    Q_D(MarkdownEditor);

    d->showUnbreakableSpaces = enabled;
}

void MarkdownEditor::setupPaperMargins()
{
    Q_D(MarkdownEditor);

    this->setViewportMargins(0, 20, 0, 0);
    d->preferredLayout->setContentsMargins(0, 0, 0, 0);

    // Use a simple monospace font at a fixed size to determine
    // margins, since getting the primary screen's width with dual monitors
    // will not account for differing DPIs in case the window is moved
    // to a different screen.
    //
    QFont f;
    f.setStyleHint(QFont::Monospace);
    f.setFamily("Courier New");
    f.setPointSize(12);

    int width = QFontMetrics(f).horizontalAdvance('@');

    switch (d->editorWidth) {
    case EditorWidthNarrow:
        width *= 60;
        break;
    case EditorWidthMedium:
        width *= 80;
        break;
    case EditorWidthWide:
        width *= 100;
        break;
    default:
        return;
    }

    int margin = 0;

    if (width <= this->viewport()->width()) {
        margin = (this->viewport()->width() - width) / 2;
    }

    this->setViewportMargins(margin, 20, margin, 0);
}

QVariant MarkdownEditor::inputMethodQuery(Qt::InputMethodQuery query) const
{
    switch (query)
    {
    case Qt::ImCursorRectangle:
    {
        QFontMetrics metrics(font());
        QRect r = cursorRect();
        r.translate(contentOffset().toPoint());
        r.translate(viewportMargins().left(), 0);
        r.adjust(0, metrics.ascent(), 0, metrics.ascent());
        return r;
    }
    default:
        return QPlainTextEdit::inputMethodQuery(query);
    }
}

QRect MarkdownEditor::cursorRect(const QTextCursor &cursor) const
{
    QRect r = QPlainTextEdit::cursorRect(cursor);
    r.setWidth(MarkdownEditorPrivate::CursorWidth);
    return r;
}

QRect MarkdownEditor::cursorRect() const
{
    QRect r = QPlainTextEdit::cursorRect();
    r.setWidth(MarkdownEditorPrivate::CursorWidth);
    return r;
}

int MarkdownEditor::cursorWidth() const
{
    return MarkdownEditorPrivate::CursorWidth;
}

bool MarkdownEditor::canInsertFromMimeData(const QMimeData *source) const
{
    return source->hasImage() || QPlainTextEdit::canInsertFromMimeData(source);
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
    Q_D(MarkdownEditor);
    
    if (e->mimeData()->hasUrls() && (e->mimeData()->urls().size() == 1)) {
        e->acceptProposedAction();

        QUrl url = e->mimeData()->urls().first();
        QString path = url.toLocalFile();
        bool isRelativePath = false;

        QFileInfo fileInfo(path);
        QString fileExtension = fileInfo.suffix().toLower();

        QTextCursor dropCursor = cursorForPosition(e->position().toPoint());

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
            if (!d->textDocument->isNew()) {
                QFileInfo docInfo(d->textDocument->filePath());

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
            QDropEvent *dummyEvent = new QDropEvent(e->position().toPoint(), e->possibleActions(), dummyMimeData, e->buttons(), e->modifiers());
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

void MarkdownEditor::insertFromMimeData(const QMimeData *source)
{
    Q_D(MarkdownEditor);

    if (source->hasImage()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        QString imagePath, startingDirectory;

        QString documentName = QFileInfo(d->textDocument->filePath()).baseName();
        if (!d->textDocument->isNew()) {
            startingDirectory = QFileInfo(d->textDocument->filePath()).dir().path();
            imagePath = startingDirectory + "/" + documentName + "_";
        } else {
            imagePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
        }

        imagePath += QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png";

        imagePath = QFileDialog::getSaveFileName(
            this,
            tr("Save Image"),
            imagePath,
            d->imageSaveFilter
        );

        if (!imagePath.isNull() && !imagePath.isEmpty()) {
            // Write the image to the path selected by the user
            QImageWriter writer;
            writer.setFileName(imagePath);

            if (!writer.write(image)) {
                QMessageBox::critical(this,
                    qApp->applicationName(),
                    writer.errorString());
                QPlainTextEdit::insertFromMimeData(source);
                return;
            }

            QFileInfo imgInfo(imagePath);
            bool isRelativePath = false;

            if (imgInfo.exists()) {
                if (!d->textDocument->isNew()) {
                    QFileInfo docInfo(d->textDocument->filePath());

                    if (docInfo.exists()) {
                        imagePath = docInfo.dir().relativeFilePath(imagePath);
                        isRelativePath = true;
                    }
                }
            }

            if (!isRelativePath) {
                imagePath = QString("file://") + imagePath;
            }

            QTextCursor cursor = (this->textCursor());
            cursor.insertText(QString("![](%1)").arg(imagePath));
        }
    } else {
        QPlainTextEdit::insertFromMimeData(source);
    }
}

/*
* This method contains a code snippet that was lifted and modified from ReText
*/
void MarkdownEditor::keyPressEvent(QKeyEvent *e)
{
    Q_D(MarkdownEditor);
    
    int key = e->key();

    QTextCursor cursor(this->textCursor());

    switch (key) {
    case Qt::Key_Return:
        if (!cursor.hasSelection()) {
            if (e->modifiers() & Qt::ShiftModifier) {
                // Insert Markdown-style line break
                cursor.insertText("  ");
                d->highlighter->rehighlightBlock(cursor.block());
            }

            if (e->modifiers() & Qt::ControlModifier) {
                cursor.insertText("\n");
            } else {
                d->handleCarriageReturn();
            }
        } else {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    case Qt::Key_Delete:
        if (!d->hemingwayModeEnabled) {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    case Qt::Key_Backspace:
        if (!d->hemingwayModeEnabled) {
            if (!d->handleBackspaceKey()) {
                QPlainTextEdit::keyPressEvent(e);
            }
        }
        break;
    case Qt::Key_Tab:
        if (!d->handleWhitespaceInEmptyMatch('\t')) {
            indentText();
        }
        break;
    case Qt::Key_Backtab:
        unindentText();
        break;
    case Qt::Key_Space:
        if (!d->handleWhitespaceInEmptyMatch(' ')) {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    default:
        if (e->text().size() == 1) {
            QChar ch = e->text().at(0);

            if (!d->handleEndPairCharacterTyped(ch) && !d->insertPairedCharacters(ch)) {
                QPlainTextEdit::keyPressEvent(e);
            }
        } else {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    }
}

void MarkdownEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(MarkdownEditor);

    d->mouseButtonDown = true;
    QPlainTextEdit::mouseDoubleClickEvent(e);
}

void MarkdownEditor::mousePressEvent(QMouseEvent *e)
{
    Q_D(MarkdownEditor);

    d->mouseButtonDown = true;
    QPlainTextEdit::mousePressEvent(e);
}

void MarkdownEditor::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(MarkdownEditor);

    d->mouseButtonDown = false;
    QPlainTextEdit::mouseReleaseEvent(e);
}

void MarkdownEditor::wheelEvent(QWheelEvent *e)
{    
    Qt::KeyboardModifiers modifier = e->modifiers();

    int numDegrees = 0;

    QPoint angleDelta = e->angleDelta();

    if (!angleDelta.isNull()) {
        numDegrees = angleDelta.y();
    }

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
    Q_D(MarkdownEditor);

    InlineMarkupToggle toggle(MarkdownNode::Strong);
    toggle(this);
}

void MarkdownEditor::italic()
{
    Q_D(MarkdownEditor);
    
    InlineMarkupToggle toggle(MarkdownNode::Emph);
    toggle(this);
}

void MarkdownEditor::strikethrough()
{
    Q_D(MarkdownEditor);
    
    InlineMarkupToggle toggle(MarkdownNode::Strikethrough);
    toggle(this);
}

void MarkdownEditor::insertCodeFences()
{
    QTextCursor cursor = this->textCursor();
    QString text;

    if (cursor.block().position() != cursor.position()) {
        text = "\n";
    }

    if (cursor.hasSelection()) {
        QString selText = cursor.selectedText();
        text += QString("```\n" + selText + "\n```\n");
        cursor.insertText(text);
    } else {
        text += QString("```\n\n```\n");
        cursor.insertText(text);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 5);
        this->setTextCursor(cursor);
    }
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
    Q_D(MarkdownEditor);
    
    d->insertPrefixForBlocks("* ");
}

void MarkdownEditor::createBulletListWithMinusMarker()
{
    Q_D(MarkdownEditor);
    
    d->insertPrefixForBlocks("- ");
}

void MarkdownEditor::createBulletListWithPlusMarker()
{
    Q_D(MarkdownEditor);
    
    d->insertPrefixForBlocks("+ ");
}

void MarkdownEditor::createNumberedListWithPeriodMarker()
{
    Q_D(MarkdownEditor);
    
    d->createNumberedList('.');
}

void MarkdownEditor::createNumberedListWithParenthesisMarker()
{
    Q_D(MarkdownEditor);
    
    d->createNumberedList(')');
}

void MarkdownEditor::createTaskList()
{
    Q_D(MarkdownEditor);
    
    d->insertPrefixForBlocks("- [ ] ");
}

void MarkdownEditor::createBlockquote()
{
    Q_D(MarkdownEditor);
    
    d->insertPrefixForBlocks("> ");
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
    Q_D(MarkdownEditor);
    
    QTextCursor cursor = this->textCursor();

    if (cursor.hasSelection()) {
        QTextBlock block = this->document()->findBlock(cursor.selectionStart());
        QTextBlock end = this->document()->findBlock(cursor.selectionEnd()).next();

        cursor.beginEditBlock();

        while (block != end) {
            cursor.setPosition(block.position());

            if (d->insertSpacesForTabs) {
                QString indentText = "";

                for (int i = 0; i < d->tabWidth; i++) {
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
        int indent = d->tabWidth;
        QString indentText = "";
        QRegularExpressionMatch match;

        cursor.beginEditBlock();

        switch (cursor.block().userState() & MarkdownStateMask) {
        case MarkdownStateNumberedList:
            match = d->emptyNumberedListRegex.match(cursor.block().text());

            if (match.hasMatch()) {
                QStringList capture = match.capturedTexts();

                // Restart numbering for the nested list.
                if (capture.size() == 2) {
                    static QRegularExpression numberRegex("\\d+");

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
            if (d->emptyTaskListRegex.match(cursor.block().text()).hasMatch()) {
                cursor.movePosition(QTextCursor::StartOfBlock);
            }
            break;
        case MarkdownStateBulletPointList: {
            if (d->emptyBulletListRegex.match(cursor.block().text()).hasMatch()) {
                if (d->bulletPointCyclingEnabled) {
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
            indent = d->tabWidth - (cursor.positionInBlock() % d->tabWidth);
            break;
        }

        if (d->insertSpacesForTabs) {
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
    Q_D(MarkdownEditor);
    
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
                && (pos < d->tabWidth)
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
        && (d->emptyBulletListRegex.match(cursor.block().text()).hasMatch())
        && d->bulletPointCyclingEnabled
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

void MarkdownEditor::deselectText()
{
    QTextCursor cursor = textCursor();

    cursor.clearSelection();
    setTextCursor(cursor);
}

bool MarkdownEditor::toggleTaskComplete()
{
    Q_D(MarkdownEditor);
    
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
            && (block.text().indexOf(d->taskListRegex, 0, &match) == 0)
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

void MarkdownEditor::insertImage()
{
    Q_D(MarkdownEditor);

    QString startingDirectory = QString();
    MarkdownDocument *document = (MarkdownDocument*) this->document();

    if (!document->isNew()) {
        startingDirectory = QFileInfo(document->filePath()).dir().path();
    }

    QString imagePath =
        QFileDialog::getOpenFileName(
            this,
            tr("Insert Image"),
            startingDirectory,
            d->imageOpenFilter
        );

    if (!imagePath.isNull() && !imagePath.isEmpty()) {
        QFileInfo imgInfo(imagePath);
        bool isRelativePath = false;

        if (imgInfo.exists()) {
            if (!document->isNew()) {
                QFileInfo docInfo(document->filePath());

                if (docInfo.exists()) {
                    imagePath = docInfo.dir().relativeFilePath(imagePath);
                    isRelativePath = true;
                }
            }
        }

        if (!isRelativePath) {
            imagePath = QString("file://") + imagePath;
        }

        QTextCursor cursor = this->textCursor();
        cursor.insertText(QString("![](%1)").arg(imagePath));
    }
}

void MarkdownEditor::setEnableLargeHeadingSizes(bool enable)
{
    Q_D(MarkdownEditor);
    
    d->highlighter->setEnableLargeHeadingSizes(enable);
}

void MarkdownEditor::setAutoMatchEnabled(bool enable)
{
    Q_D(MarkdownEditor);
    
    d->autoMatchEnabled = enable;
}

void MarkdownEditor::setAutoMatchEnabled(const QChar openingCharacter, bool enabled)
{
    Q_D(MarkdownEditor);
    
    d->autoMatchFilter.insert(openingCharacter, enabled);
}

void MarkdownEditor::setBulletPointCyclingEnabled(bool enable)
{
    Q_D(MarkdownEditor);
    
    d->bulletPointCyclingEnabled = enable;
}

void MarkdownEditor::setUseUnderlineForEmphasis(bool enable)
{
    Q_D(MarkdownEditor);
    
    d->highlighter->setUseUnderlineForEmphasis(enable);
}

void MarkdownEditor::setItalicizeBlockquotes(bool enable)
{
    Q_D(MarkdownEditor);
    
    d->highlighter->setItalicizeBlockquotes(enable);
}

void MarkdownEditor::setInsertSpacesForTabs(bool enable)
{
    Q_D(MarkdownEditor);
    
    d->insertSpacesForTabs = enable;
}

void MarkdownEditor::setTabulationWidth(int width)
{
    Q_D(MarkdownEditor);
    
    QFontMetrics fontMetrics(font());
    d->tabWidth = width;

#if (QT_VERSION_MAJOR == 5) && (QT_VERSION_MINOR < 10)
    this->setTabStopWidth(fontMetrics.width(' ') * d->tabWidth);
#else
    this->setTabStopDistance(fontMetrics.horizontalAdvance(' ') * d->tabWidth);
#endif
}

void MarkdownEditor::setEditorWidth(EditorWidth width)
{
    Q_D(MarkdownEditor);
    
    d->editorWidth = width;
}

void MarkdownEditor::setEditorCorners(InterfaceStyle corners)
{
    Q_D(MarkdownEditor);
    
    d->editorCorners = corners;
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

void MarkdownEditor::onContentsChanged(int position, int charsRemoved, int charsAdded)
{
    Q_D(MarkdownEditor);
    
    Q_UNUSED(position)
    Q_UNUSED(charsRemoved)
    Q_UNUSED(charsAdded)

    if (d->loadingDocument) {
        return;
    }

    d->parseDocument();

    // Don't use the textChanged() or contentsChanged() (no parameters) signals
    // for checking if the typingResumed() signal needs to be emitted.  These
    // two signals are emitted even when the text formatting changes (i.e.,
    // when the QSyntaxHighlighter formats the text). Instead, use QTextDocument's
    // contentsChange(int, int, int) signal, which is only emitted when the
    // document text actually changes.
    //
    if (d->typingHasPaused || d->scaledTypingHasPaused) {
        d->typingHasPaused = false;
        d->scaledTypingHasPaused = false;
        d->typingPausedSignalSent = false;
        d->typingPausedScaledSignalSent = false;
        emit typingResumed();
    }
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
    Q_D(MarkdownEditor);
    
    if (FocusModeDisabled != d->focusMode) {
        QTextEdit::ExtraSelection beforeFadedSelection;
        QTextEdit::ExtraSelection afterFadedSelection;
        beforeFadedSelection.format.setForeground(d->fadeColor);
        beforeFadedSelection.cursor = this->textCursor();
        afterFadedSelection.format.setForeground(d->fadeColor);
        afterFadedSelection.cursor = this->textCursor();

        bool canFadePrevious = false;

        QList<QTextEdit::ExtraSelection> selections;

        switch (d->focusMode) {
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
        // `FocusModeTypewriter` implicitly handled here as we don't highlight anything but center the current line.
        default:
            break;
        }

        this->setExtraSelections(selections);
    }
}

void MarkdownEditor::checkIfTypingPaused()
{
    Q_D(MarkdownEditor);

    if (!d->loadingDocument && d->typingHasPaused && !d->typingPausedSignalSent) {
        d->typingPausedSignalSent = true;
        emit typingPaused();
    }

    d->typingTimer->stop();
    d->typingTimer->start(1000);

    d->typingHasPaused = true;
}

void MarkdownEditor::checkIfTypingPausedScaled()
{
    Q_D(MarkdownEditor);
    
    if (!d->loadingDocument && d->scaledTypingHasPaused && !d->typingPausedScaledSignalSent) {
        d->typingPausedScaledSignalSent = true;
        emit typingPausedScaled();
    }

    // Scale timer interval based on document size.
    int interval = (document()->characterCount() / 30000) * 20;

    if (interval > 1000) {
        interval = 1000;
    } else if (interval < 20) {
        interval = 20;
    }

    d->scaledTypingTimer->stop();
    d->scaledTypingTimer->start(interval);

    d->scaledTypingHasPaused = true;
}

void MarkdownEditor::onCursorPositionChanged()
{
    Q_D(MarkdownEditor);
    
    if (!d->mouseButtonDown) {
        QRect cursor = this->cursorRect();
        QRect viewport = this->viewport()->rect();
        int bottom = viewport.bottom() - this->fontMetrics().height();

        if ((d->focusMode != FocusModeDisabled)
                || (cursor.bottom() >= bottom)
                || (cursor.top() <= viewport.top())) {
            centerCursor();
        }
    }

    // Set the text cursor back to visible and reset the blink timer so that
    // the cursor is always visible whenever it moves to a new position.
    //
    d->textCursorVisible = true;
    d->cursorBlinkTimer->stop();
    d->cursorBlinkTimer->start();

    // Update widget to ensure cursor is drawn.
    update();

    emit cursorPositionChanged(this->textCursor().position());
}

void MarkdownEditorPrivate::toggleCursorBlink()
{
    Q_Q(MarkdownEditor);
    
    this->textCursorVisible = !this->textCursorVisible;
    q->update();
}

void MarkdownEditorPrivate::parseDocument()
{
    Q_Q(MarkdownEditor);

    parseText(q->document()->toPlainText());
}

void MarkdownEditorPrivate::parseText(const QString &text)
{
    Q_Q(MarkdownEditor);

    MarkdownAST *ast = CmarkGfmAPI::instance()->parse(text, false);

    // Note:  MarkdownDocument is responsible for freeing memory
    // allocated for the AST.
    //
    ((MarkdownDocument *) q->document())->setMarkdownAST(ast);
}

void MarkdownEditorPrivate::handleCarriageReturn()
{
    Q_Q(MarkdownEditor);
    
    QString autoInsertText = "";
    QTextCursor cursor = q->textCursor();
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
    q->ensureCursorVisible();
}

bool MarkdownEditorPrivate::handleBackspaceKey()
{
    Q_Q(MarkdownEditor);
    
    QTextCursor cursor = q->textCursor();

    if (cursor.hasSelection()) {
        return false;
    }

    int backtrackIndex = -1;

    switch (cursor.block().userState() & MarkdownStateMask) {
    case MarkdownStateNumberedList: {
        if (emptyNumberedListRegex.match(q->textCursor().block().text()).hasMatch()) {
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
    Q_Q(MarkdownEditor);
    
    QTextCursor cursor = q->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = q->document()->findBlock(cursor.selectionStart());
        end = q->document()->findBlock(cursor.selectionEnd()).next();
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
    Q_Q(MarkdownEditor);
    
    QTextCursor cursor = q->textCursor();
    QTextBlock block;
    QTextBlock end;

    if (cursor.hasSelection()) {
        block = q->document()->findBlock(cursor.selectionStart());
        end = q->document()->findBlock(cursor.selectionEnd()).next();
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
    Q_Q(MarkdownEditor);
    
    if (autoMatchEnabled
            && markupPairs.contains(firstChar)
            && autoMatchFilter.value(firstChar)) {
        QChar lastChar = markupPairs.value(firstChar);
        QTextCursor cursor = q->textCursor();
        QTextBlock block;
        QTextBlock end;

        if (cursor.hasSelection()) {
            block = q->document()->findBlock(cursor.selectionStart());
            end = q->document()->findBlock(cursor.selectionEnd());

            // Only surround selection with matched characters if the
            // selection belongs to the same block.
            //
            if (block == end) {
                cursor.beginEditBlock();
                cursor.setPosition(cursor.selectionStart());
                cursor.insertText(firstChar);
                cursor.setPosition(q->textCursor().selectionEnd());
                cursor.insertText(lastChar);
                cursor.endEditBlock();

                cursor = q->textCursor();
                cursor.setPosition(cursor.selectionStart());
                cursor.setPosition
                (
                    q->textCursor().selectionEnd() - 1,
                    QTextCursor::KeepAnchor
                );
                q->setTextCursor(cursor);
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
                    // characters that need matching even if preceded by
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
                q->setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

bool MarkdownEditorPrivate::handleEndPairCharacterTyped(const QChar ch)
{
    Q_Q(MarkdownEditor);
    
    QTextCursor cursor = q->textCursor();

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
        QTextCursor cursor = q->textCursor();
        QString text = cursor.block().text();
        int pos = cursor.positionInBlock();

        if (pos < (text.length())) {
            // Look ahead to the character after the cursor position. If it
            // matches the character that was entered, then move the cursor
            // one position forward.
            //
            if (text[pos] == ch) {
                cursor.movePosition(QTextCursor::NextCharacter);
                q->setTextCursor(cursor);
                return true;
            }
        }
    }

    return false;
}

bool MarkdownEditorPrivate::handleWhitespaceInEmptyMatch(const QChar whitespace)
{
    Q_Q(MarkdownEditor);
    
    QTextCursor cursor = q->textCursor();
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

QString MarkdownEditorPrivate::priorIndentation()
{

    Q_Q(MarkdownEditor);
    
    QString indent = "";
    QTextCursor cursor = q->textCursor();
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
    Q_Q(MarkdownEditor);
    
    QTextCursor cursor = q->textCursor();
    QTextBlock block = cursor.block();

    QString text = block.text();

    if ((text.indexOf(itemRegex, 0, &match) >= 0) && match.hasMatch()) {
        return match.captured();
    }

    return QString("");
}

bool MarkdownEditorPrivate::insideBlockArea(const QTextBlock &block, BlockType &type) const
{
    if (!block.isValid()) {
        type = BlockTypeNone;
        return false;
    }

    if (isBlockquote(block)) {
        type = BlockTypeQuote;
        return true;
    }

    if (isCodeBlock(block)) {
        type = BlockTypeCode;
        return true;
    }

    type = BlockTypeNone;
    return false;
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

bool MarkdownEditorPrivate::isCodeBlock(const QTextBlock &block) const
{
    return (MarkdownStateCodeBlock == (MarkdownStateCodeBlock & block.userState()));
}

QStringList MarkdownEditorPrivate::buildImageReaderFormats()
{
    QStringList result;

    QList<QByteArray> supportedFormats = QImageReader::supportedMimeTypes();

    for (auto mimeType : webMimeTypes) {
        if (supportedFormats.contains(mimeType.toLatin1())) {
            result.append(mimeType);
        }
    }

    // Since QImageReader::supportedMimeTypes does not return the SVG mime
    // type even though Qt supports reading it and we have included that
    // module to build, add it manually.
    result.append(QStringLiteral("image/svg+xml"));

    return result;
}

QStringList MarkdownEditorPrivate::buildImageWriterFormats()
{
    QStringList result;

    QList<QByteArray> supportedFormats = QImageWriter::supportedMimeTypes();

    for (auto mimeType : webMimeTypes) {
        if (supportedFormats.contains(mimeType.toLatin1())) {
            result.append(mimeType);
        }
    }

    return result;
}

QString MarkdownEditorPrivate::buildImageFilters(
    const QStringList &mimeTypes,
    bool includeWildcardImages)
{
    QMimeDatabase db;
    QString allFileExtensions = "";
    QStringList filters;

    for (const QString &mimeType : mimeTypes) {
        QMimeType mime(db.mimeTypeForName(mimeType));
        const QString patterns = mime.globPatterns().join(QLatin1Char(' '));
        filters.append(mime.comment() + QLatin1String(" (") + patterns + QLatin1String(")"));

        if (includeWildcardImages) {
            allFileExtensions += patterns + QLatin1Char(' ');
        }
    }

    if (includeWildcardImages) {
        filters.insert(0, MarkdownEditor::tr("Images (%1)").arg(allFileExtensions.trimmed()));
    }

    return filters.join(";;");
}

} // namespace ghostwriter
