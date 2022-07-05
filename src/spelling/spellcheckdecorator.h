/***********************************************************************
 *
 * Copyright (C) 2014-2022 wereturtle
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

#ifndef SPELL_CHECK_DECORATOR_H
#define SPELL_CHECK_DECORATOR_H

#include <QEvent>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>

#include "markdownhighlighter.h"

namespace ghostwriter
{
class SpellCheckDecoratorPrivate;
class SpellCheckDecorator : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SpellCheckDecorator)

public:
    /**
     * Constructor.
     */
    SpellCheckDecorator(QPlainTextEdit *editor);

    /**
     * Destructor.
     */
    ~SpellCheckDecorator();

    /**
     * Returns whether live spell checking is enabled.
     */
    bool spellCheckEnabled() const;

public slots:
    /**
     * Sets whether live spell checking is enabled.
     */
    void setSpellCheckEnabled(bool enabled);

    /**
     * Sets the dictionary language to use for spell checking.
     */
    void setDictionary(const QString &language);

    /**
     * Runs the interactive spell checker dialog.
     */
    void runSpellCheck();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QScopedPointer<SpellCheckDecoratorPrivate> d_ptr;
};
} // namespace ghostwriter

#endif