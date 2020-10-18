/***********************************************************************
 *
 * Copyright (C) 2017-2020 wereturtle
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

#ifndef PREVIEWOPTIONSDIALOG_H
#define PREVIEWOPTIONSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace ghostwriter
{
/**
 * Displays HTML preview options.
 */
class PreviewOptionsDialogPrivate;
class PreviewOptionsDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PreviewOptionsDialog)

public:
    /**
     * Constructor.
     */
    PreviewOptionsDialog(QWidget *parent = NULL);

    /**
     * Destructor.
     */
    virtual ~PreviewOptionsDialog();

private:
    QScopedPointer<PreviewOptionsDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // PREVIEWOPTIONSDIALOG_H
