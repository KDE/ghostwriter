/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "theme.h"

namespace ghostwriter
{

/**
 * Dialog that allows the user to select a different theme, add a new theme,
 * delete a theme, or edit a theme.
 */
class ThemeSelectionDialogPrivate;
class ThemeSelectionDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ThemeSelectionDialog)

public:
    /**
     * Constructor.  Takes as a parameter the name of the theme that is
     * currently styling the application.
     */
    ThemeSelectionDialog
    (
        const QString &currentThemeName,
        const bool darkModeEnabled,
        QWidget *parent = nullptr
    );

    /**
     * Destructor.
     */
    ~ThemeSelectionDialog();

    /**
     * Gets the currently selected theme.
     */
    const Theme &theme() const;

private:
    QScopedPointer<ThemeSelectionDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // THEMESELECTIONDIALOG_H
