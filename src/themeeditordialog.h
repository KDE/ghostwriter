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

#ifndef THEMEEDITORDIALOG_H
#define THEMEEDITORDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "theme.h"

namespace ghostwriter
{
/**
 * Theme editor dialog which allows the user modify a theme for the
 * application's look and feel.
 */
class ThemeEditorDialogPrivate;
class ThemeEditorDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ThemeEditorDialog)

public:
    /**
     * Constructor.  Takes the theme to to be edited as a parameter.
     */
    ThemeEditorDialog(const Theme &theme, QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~ThemeEditorDialog();

    /**
     * Returns the theme selected if action was to accept.
     */
    const Theme &theme() const;

private slots:
    void accept();
    void reject();

private:
    QScopedPointer<ThemeEditorDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // THEMEEDITORDIALOG_H
