/***********************************************************************
 *
 * Copyright (C) 2014-2021 wereturtle
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

#ifndef SIMPLEFONTDIALOG_H
#define SIMPLEFONTDIALOG_H

#include <QDialog>
#include <QFont>
#include <QScopedPointer>

namespace ghostwriter
{
/**
 * A simplified font dialog displaying only the font family and font size as
 * options for the user to choose.  This class will also render a preview
 * of the font.
 */
class SimpleFontDialogPrivate;
class SimpleFontDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SimpleFontDialog)

public:
    /**
     * Constructor.
     */
    SimpleFontDialog(QWidget *parent = 0);

    /**
     * Constructor.  Takes initial font to display as a parameter.
     */
    SimpleFontDialog(const QFont &initial, QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~SimpleFontDialog();

    /**
     * Sets whether only monospace are shown (true), or whether all fonts are
     * shown (false).  The option to toggle the setting with a checkbox can
     * be hidden from the user by setting hideCheckbox to true.
     */
    void setMonospaceOnly(bool available, bool hideCheckbox = false);

    /**
     * Returns whether only monospace are shown (true), or whether all fonts
     * are shown (false).
     */
    bool monospaceOnly() const;

    /**
     * Gets the currently selected font.  Call this method after exec()
     * returns.
     */
    QFont selectedFont() const;

    /**
     * Convenience method to create the dialog and extract the font in
     * one easy call.  Takes initial font to display as a parameter.
     */
    static QFont font
    (
        bool *ok,
        const QFont &initial,
        QWidget *parent = nullptr
    );

    /**
     * Convenience method to create the dialog and extract the font in
     * one easy call.
     */
    static QFont font(bool *ok, QWidget *parent = nullptr);

    /**
     * Convenience method to create the dialog and extract the font in
     * one easy call.  Takes initial font to display as a parameter.
     * Only shows monospace fonts as available choices.
     */
    static QFont monospaceFont
    (
        bool *ok,
        const QFont &initial,
        QWidget *parent = nullptr
    );

    /**
     * Convenience method to create the dialog and extract the font in
     * one easy call.  Only shows monospace fonts as available choices.
     */
    static QFont monospaceFont(bool *ok, QWidget *parent = nullptr);

private:
    QScopedPointer<SimpleFontDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // SIMPLEFONTDIALOG_H
