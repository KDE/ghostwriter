/*
 * SPDX-FileCopyrightText: 2018-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDesktopServices>

#include "sandboxedwebpage.h"

namespace ghostwriter
{
SandboxedWebPage::SandboxedWebPage(QObject *parent)
    : QWebEnginePage(parent)
{

}

SandboxedWebPage::~SandboxedWebPage()
{
    ;
}

bool SandboxedWebPage::acceptNavigationRequest
(
    const QUrl &url,
    QWebEnginePage::NavigationType type,
    bool isMainFrame
)
{
    Q_UNUSED(isMainFrame)

    if (QWebEnginePage::NavigationTypeLinkClicked == type) {
        QDesktopServices::openUrl(url);
        return false;
    } else {
        return true;
    }
}
} // namespace ghostwriter
