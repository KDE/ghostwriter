/***********************************************************************
 *
 * Copyright (C) 2018-2022 wereturtle
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

#ifndef PREVIEWPROXY_H
#define PREVIEWPROXY_H

#include <QObject>
#include <QString>

namespace ghostwriter
{
/**
 * Web Channel Proxy to the HTML preview data. Object is shared between C++ and
 * JavaScript to pass the live preview's settings/data updates.
 */
class PreviewProxy : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    explicit PreviewProxy(QObject *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~PreviewProxy();

    /**
     * Sets the HTML contents of the live preview browser.
     */
    void setHtmlContent(const QString &html);

    /**
     * Returns the HTML contents of the live preview browser.
     */
    Q_INVOKABLE QString htmlContent() const;
    Q_PROPERTY(QString htmlContent READ htmlContent NOTIFY htmlChanged);

    /**
     * Sets the CSS style sheet of the live preview browser.
     */
    void setStyleSheet(const QString &css);

    /**
     * Returns the CSS style sheet used in the live preview browser.
     */
    Q_INVOKABLE QString styleSheet() const;
    Q_PROPERTY(QString styleSheet READ styleSheet NOTIFY styleSheetChanged);

    /**
     * Sets whether math rendering is enabled in the live preview.
     */
    void setMathEnabled(bool enabled);

    /**
     * Returns true if math rendering is enabled in the live preview,
     * false otherwise.
     */
    Q_INVOKABLE bool mathEnabled() const;
    Q_PROPERTY(bool mathEnabled READ mathEnabled NOTIFY mathToggled);

signals:
    /**
     * Emitted when the HTML content changes.
     */
    void htmlChanged(const QString &html);

    /**
     * Emitted when the style sheet changes.
     */
    void styleSheetChanged(const QString &css);

    /**
     * Emitted when the math rendering is toggled.
     */
    void mathToggled(bool enabled);

private:
    QString m_htmlContent;
    QString m_styleSheet;
    bool m_mathEnabled;
};

Q_DECLARE_METATYPE(PreviewProxy);

} // namespace ghostwriter

#endif // PREVIEWPROXY_H
