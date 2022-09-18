/*
 * SPDX-FileCopyrightText: 2018-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

} // namespace ghostwriter

#endif // PREVIEWPROXY_H
