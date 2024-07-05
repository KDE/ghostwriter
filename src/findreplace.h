/*
 * SPDX-FileCopyrightText: 2020-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QWidget>

namespace ghostwriter
{
/**
 * Find/Replace widget intended to be placed at the top or bottom of a window.
 * It presently only supports searches for the QPlaintTextEdit widget.
 */
class FindReplacePrivate;
class FindReplace : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FindReplace)

public:
    /**
     * Constructor.
     */
    FindReplace(QPlainTextEdit *editor, QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~FindReplace();

    void setRegexSearchIcon(const QIcon &icon);
    void setMatchCaseIcon(const QIcon &icon);
    void setWholeWordIcon(const QIcon &icon);
    void setHighlightMatchesIcon(const QIcon &icon);
    void setFindNextIcon(const QIcon &icon);
    void setFindPreviousIcon(const QIcon &icon);
    void setReplaceIcon(const QIcon &icon);
    void setReplaceAllIcon(const QIcon &icon);
    void setCloseIcon(const QIcon &icon);

    /**
     * PRIVATE USE ONLY.
     * 
     * This method is overridden to allow this widget to control
     * the change of focus between child widgets when the tab
     * key is pressed.
     */
    bool focusNextPrevChild(bool next) override;

    /**
     * PRIVATE USE ONLY.
     * 
     * This method is overridden to respond to common shortcut
     * keys used in a find/replace widget.
     */
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    // NOTE:  The following slots should be triggered by shortcut keys.
    //        For example, you can connect the CTRL+F shortcut to the
    //        showFindView() slot.
    
    /**
     * Shows the find view of the widget without the replace view.
     */
    void showFindView();

    /**
     * Shows both the find and replace view of the widget.
     */
    void showReplaceView();

    /**
     * Finds the next occurrence of the search query in the find field.
     */
    void findNext();

    /**
     * Finds the previous occurrence of the search query in the find field.
     */
    void findPrevious();

    /**
     * Replaces the currently selected text in the editor with the value
     * in the replace field.
     */
    void replace();

    /**
     * Replaces all occurrences in the editor of the text in the find field
     * with the text in the replace field.
     */
    void replaceAll();

private:
    QScopedPointer<FindReplacePrivate> d_ptr;
};
}


#endif // FINDREPLACE_H
