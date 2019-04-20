/***********************************************************************
 *
 * Copyright (C) 2014-2019 wereturtle
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

#include <QApplication>
#include <QCoreApplication>
#include <QtWebEngine/qtwebengineglobal.h>
#include <QTranslator>
#include <QLocale>

#include "MainWindow.h"
#include "AppSettings.h"

int main(int argc, char* argv[])
{
#if QT_VERSION >= 0x050600
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if QT_VERSION >= 0x050700 && defined(Q_OS_LINUX)
    QGuiApplication::setDesktopFileName("ghostwriter");
#endif

    QApplication app(argc, argv);
    QtWebEngine::initialize();

    // Call this to force settings initialization before the application
    // fully launches.
    //
    AppSettings* appSettings = AppSettings::getInstance();
    QLocale::setDefault(appSettings->getLocale());

    QTranslator qtTranslator;
    bool ok = qtTranslator.load("qt_" + appSettings->getLocale(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    if (!ok)
    {
        qtTranslator.load("qt_" + appSettings->getLocale(),
            appSettings->getTranslationsPath());
    }

    app.installTranslator(&qtTranslator);

    QTranslator qtBaseTranslator;
    ok = qtBaseTranslator.load("qtbase_" + appSettings->getLocale(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    if (!ok)
    {
        qtBaseTranslator.load("qtbase_" + appSettings->getLocale(),
            appSettings->getTranslationsPath());
    }

    app.installTranslator(&qtBaseTranslator);

    QTranslator appTranslator;
    ok = appTranslator.load
    (
        QString("ghostwriter_") + appSettings->getLocale(),
        appSettings->getTranslationsPath()
    );

    if (!ok)
    {
        appTranslator.load
        (
            "ghostwriter_en",
            appSettings->getTranslationsPath()
        );
    }

    app.installTranslator(&appTranslator);

    QString filePath = QString();

    if (argc > 1)
    {
        filePath = app.arguments().at(1);
    }

    MainWindow window(filePath);

    window.show();
    return app.exec();
}
