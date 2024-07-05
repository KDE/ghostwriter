/**
 * SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGICONTHEME_H
#define SVGICONTHEME_H

#include <QIcon>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace ghostwriter
{
/**
 * Custom renders SVG icons with the given colors for each icon mode.
 * It is recommended to use SVG icons that are 100% black with this class.
 * Icons from the given icon theme directory path are specified by their base
 * file name, minus the .svg extension when calling icon() and checkableIcon().
 */
class SvgIconThemePrivate;
class SvgIconTheme
{
public:
    /**
     * Constructor. Takes the icon theme directory path where the SVG
     * icons are stored as a parameter.
     */
    SvgIconTheme(const QString &themePath);

    /**
     * Destructor.
     */
    ~SvgIconTheme();

    /**
     * Returns the icon theme directory path.
     */
    QString path() const;

    /**
     * Sets the color for the given QIcon mode with which to pain the icon.
     */
    void setColor(QIcon::Mode mode, const QColor &color);

    /**
     * Returns the color for the given QIcon mode.
     */
    QColor color(QIcon::Mode mode) const;

    /**
     * Returns a QIcon for the icon with the given name in this object's
     * icon theme directory.
     *
     * Be sure to specify the colors for each QIcon state with calls to
     * setColor() before calling this method.
     *
     * Note: DO NOT specify the directory path or the .svg extension.  Only the
     * base name of the icon file should be specified.
     *
     */
    QIcon icon(const QString &name) const;

    /**
     * Returns a checkable QIcon for the icon with the given checked state
     * and unchecked state names in this object's icon theme directory.
     *
     * Be sure to specify the colors for each QIcon state with calls to
     * setColor() before calling this method.
     *
     * Note: DO NOT specify the directory path or the .svg extension.  Only the
     * base name of the icon file should be specified.
     */
    QIcon checkableIcon(const QString &uncheckedName, const QString &checkedName) const;

private:
    QScopedPointer<SvgIconThemePrivate> d;
};
} // namespace ghostwriter

#endif // SVGICONTHEME_H
