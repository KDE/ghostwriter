/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SPELL_CHECK_DIALOG_H
#define SPELL_CHECK_DIALOG_H

#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>

#include <Sonnet/Dialog>

namespace ghostwriter
{
/**
 * A spell check dialog that uses Sonnet's dialog against a QPlainTextEdit.
 * 
 * NOTE: This dialog is already set to delete on close!
 */
class SpellCheckDialogPrivate;
class SpellCheckDialog : public Sonnet::Dialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SpellCheckDialog)

public:
    /**
     * Constructor.  The editor will also be used as the dialog's parent.
     */
    SpellCheckDialog(QPlainTextEdit *editor);

    /**
     * Destructor.
     */
    ~SpellCheckDialog();

private:
    QScopedPointer<SpellCheckDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif