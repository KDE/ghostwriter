/***********************************************************************
 *
 * Copyright (C) 2018-2019 wereturtle
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

#include <QDesktopServices>

#include "SandboxedWebPage.h"

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
    const QUrl& url,
    QWebEnginePage::NavigationType type,
    bool isMainFrame
)
{
    Q_UNUSED(isMainFrame)

    if (QWebEnginePage::NavigationTypeLinkClicked == type)
    {
        QDesktopServices::openUrl(url);
        return false;
    }
    else
    {
        return true;
    }
}
