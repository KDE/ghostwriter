/*
 * SPDX-FileCopyrightText: 2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CHROMECOLORS_H
#define CHROMECOLORS_H

#include <QColor>
#include <QScopedPointer>

#include <editor/colorscheme.h>

namespace ghostwriter
{
class ChromeColorsPrivate;
class ChromeColors
{
public:
    typedef enum State {
        FirstState,
        NormalState = FirstState,
        ActiveState,
        HoverState,
        PressedState,
        DisabledState,
        LastState = DisabledState,
        StateCount
    } State;

    typedef enum ColorElem {
        FirstElem,
        Background = FirstElem,
        Text,
        Label,
        Fill,
        Accent,
        AccentFill,
        Link,
        Heading,
        Code,
        BlockQuote,
        Separator,
        SelectedTextFg,
        SelectedTextBg,
        Info,
        InfoFill,
        Error,
        ErrorFill,
        Warning,
        WarningFill,
        Success,
        SuccessFill,
        SecondaryBackground,
        SecondaryFill,
        TertiaryFill,
        SecondaryLabel,
        PlaceholderText,
        SelectionFg,
        SecondarySeparator,
        Grid,
        LastElem = Grid
    } ColorElem;

    ChromeColors(const ColorScheme &base);
    ChromeColors(const ChromeColors &other);
    ~ChromeColors();

    QColor color(ColorElem elem, State state = NormalState) const;

private:
    QScopedPointer<ChromeColorsPrivate> d;
};

} // namespace ghostwriter

#endif // CHROMECOLORS_H
