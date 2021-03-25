/***********************************************************************
 *
 * Copyright (C) 2020-2021 wereturtle
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

#ifndef STYLESHEETBUILDER_H
#define STYLESHEETBUILDER_H

#include <QFont>
#include <QString>

#include "colorscheme.h"

namespace ghostwriter
{
/**
 * A convenience class to generate widget stylesheets for the application
 * based on the provided color scheme.
 */
class StyleSheetBuilder
{
public:
    /**
     * Constructor.
     */
    StyleSheetBuilder(const ColorScheme &colors,
        const bool roundedCorners,
        const QFont& previewTextFont,
        const QFont& previewCodeFont);

    /**
     * Destructor.
     */
    ~StyleSheetBuilder();

    /**
     * Gets the general layout style sheet.
     */
    QString layoutStyleSheet();

    /**
     * Gets the QSplitter style sheet.
     */
    QString splitterStyleSheet();

    /**
     * Gets the QPlainTextEdit style sheet.
     */
    QString editorStyleSheet();

    /**
     * Gets the status bar style sheet.
     */
    QString statusBarStyleSheet();

    /**
     * Gets the style sheet for individual widgets within the status bar.
     */
    QString statusBarWidgetsStyleSheet();

    /**
     * Gets the QLabel style sheet used for displaying application status
     * within the status bar.
     */
    QString statusLabelStyleSheet();

    /**
     * Gets the find/replace widget status sheet.
     */
    QString findReplaceStyleSheet();

    /**
     * Gets the side bar style sheet.
     */
    QString sidebarStyleSheet();

    /**
     * Gets the style sheet for individual widgets/panes within the
     * side bar.
     */
    QString sidebarWidgetStyleSheet();

    /**
     * Gets the CSS style sheet for display in the HTML live preview.
     */
    QString htmlPreviewCss();

    /**
     * Gets the interface text color derived from the color scheme.
     */
    QColor interfaceTextColor();

    /**
     * Gets the faint text color derived from the color scheme.
     */
    QColor faintColor();

private:
    QColor m_backgroundColor;
    QColor m_foregroundColor;
    QColor m_faintColor;
    QColor m_pressedColor;
    QColor m_hoverColor;
    QColor m_selectedFgColor;
    QColor m_selectedBgColor;
    QColor m_accentColor;
    QColor m_interfaceTextColor;
    QColor m_menuBarForegroundColor;
    QColor m_menuBarBackgroundColor;
    QColor m_headingColor;
    QColor m_codeColor;
    QColor m_linkColor;
    QColor m_blockquoteColor;
    QColor m_thickBorderColor;

    QString m_scrollBarStyleSheet;
    QString m_layoutStyleSheet;
    QString m_splitterStyleSheet;
    QString m_editorStyleSheet;
    QString m_statusBarStyleSheet;
    QString m_statusBarWidgetStyleSheet;
    QString m_statusLabelStyleSheet;
    QString m_findReplaceStyleSheet;
    QString m_sidebarStyleSheet;
    QString m_sidebarWidgetStyleSheet;
    static QString m_htmlPreviewSass;
    QString m_htmlPreviewCss;
    QFont m_htmlPreviewTextFont;
    QFont m_htmlPreviewCodeFont;

    void buildScrollBarStyleSheet(const bool roundedCorners);
    void buildLayoutStyleSheet();
    void buildSplitterStyleSheet();
    void buildEditorStyleSheet();
    void buildStatusBarStyleSheet();
    void buildStatusBarWidgetStyleSheet();
    void buildStatusLabelStyleSheet();
    void buildFindReplaceStyleSheet();
    void buildSidebarStyleSheet();
    void buildSidebarWidgetStyleSheet();
    void buildHtmlPreviewCss(const bool roundedCorners);

    /**
     * Returns the luminance of this color on a scale of 0.0 (dark) to
     * 1.0 (light).  Luminance is based on how light or dark a color
     * appears to the human eye.
     */
    double luminance(const QColor &color) const;

    /**
     * Returns a new color based on the foreground color,
     * such that the new color is lightened to achieve the desired
     * contrast ratio against the given background color.
     *
     * Note:
     *
     *   This method assumes that this foreground color is darker
     *   than the background color.  Passing in a background color
     *   that is darker than this color results in
     *   this original color being returned.
     */
    QColor lightenToMatchContrastRatio
    (
        const QColor &foreground,
        const QColor &background,
        double contrastRatio
    ) const;

    QColor applyAlpha
    (
        const QColor &foreground,
        const QColor &background,
        int alpha
    );

    static inline int applyAlphaToChannel
    (
        const int foreground,
        const int background,
        const qreal alpha
    )
    {
        return (int)((foreground * alpha) + (background * (1.0 - alpha)));
    }
};
} // namespace ghostwriter

#endif // STYLESHEETBUILDER_H