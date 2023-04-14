/*
 * SPDX-FileCopyrightText: 2017-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
    PreviewOptionsDialog(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~PreviewOptionsDialog();

private:
    QScopedPointer<PreviewOptionsDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // PREVIEWOPTIONSDIALOG_H
