/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MARKDOWNEDITORTYPES_H
#define MARKDOWNEDITORTYPES_H

namespace ghostwriter
{
enum FocusMode {
    FocusModeFirst,
    FocusModeDisabled = FocusModeFirst,
    FocusModeSentence,
    FocusModeCurrentLine,
    FocusModeThreeLines,
    FocusModeParagraph,
    FocusModeTypewriter,
    FocusModeLast = FocusModeTypewriter
};

enum EditorWidth {
    EditorWidthFirst,
    EditorWidthNarrow = EditorWidthFirst,
    EditorWidthMedium,
    EditorWidthWide,
    EditorWidthFull,
    EditorWidthLast = EditorWidthFull
};

enum InterfaceStyle {
    InterfaceStyleFirst,
    InterfaceStyleRounded = InterfaceStyleFirst,
    InterfaceStyleSquare,
    InterfaceStyleLast = InterfaceStyleSquare,
};
} // namespace ghostwriter

#endif // MARKDOWNEDITORTYPES_H
