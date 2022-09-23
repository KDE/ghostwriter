/*
 * SPDX-FileCopyrightText: 2018-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SANDBOXEDWEBPAGE_H
#define SANDBOXEDWEBPAGE_H

#include <QWebEnginePage>

namespace ghostwriter
{
/**
 * Web page for use with QWebEngineView that is "sandboxed" such that
 * external links cannot be visited without launching the default system
 * browser.
 */
class SandboxedWebPage : public QWebEnginePage
{
public:
    /**
     * Constructor.
     */
    SandboxedWebPage
    (
        QObject *parent = 0
    );

    /**
     * Destructor.
     */
    virtual ~SandboxedWebPage();

    /**
     * Handles link clicks and opens external links with the
     * default system browser.
     */
    bool acceptNavigationRequest
    (
        const QUrl &url,
        QWebEnginePage::NavigationType type,
        bool isMainFrame
    );

};
} // namespace ghostwriter

#endif // SANDBOXEDWEBPAGE_H
