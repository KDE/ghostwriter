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

#ifndef SANDBOXEDWEBPAGE_H
#define SANDBOXEDWEBPAGE_H

#include <QWebEnginePage>

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
            QObject* parent = 0
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
            const QUrl& url,
            QWebEnginePage::NavigationType type,
            bool isMainFrame
        );

};
#endif // SANDBOXEDWEBPAGE_H
