/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#include <QDebug>

#include "mainwindow.h"
#include "appsettings.h"

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050600
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QtWebEngine::initialize();
    QApplication app(argc, argv);
    
#if defined(Q_OS_WIN)
    // Use ANGLE instead of OpenGL to bypass bug where full screen windows
    // under Windows 10 and OpenGL will not show menus from the menu bar
    // (or any other popup menus).  Thank you, Microsoft, you made my day.
    //
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES, true);
#endif

#if QT_VERSION >= 0x050700 && defined(Q_OS_LINUX)
    QGuiApplication::setDesktopFileName("ghostwriter");
#endif

    // Call this to force settings initialization before the application
    // fully launches.
    //
    ghostwriter::AppSettings *appSettings = ghostwriter::AppSettings::instance();
    QLocale::setDefault(appSettings->locale());

    QTranslator qtTranslator;
    bool ok = qtTranslator.load("qt_" + appSettings->locale(),
                                QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    if (!ok) {
        qtTranslator.load("qt_" + appSettings->locale(),
                          appSettings->translationsPath());
    }

    app.installTranslator(&qtTranslator);

    QTranslator qtBaseTranslator;
    ok = qtBaseTranslator.load("qtbase_" + appSettings->locale(),
                               QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    if (!ok) {
        qtBaseTranslator.load("qtbase_" + appSettings->locale(),
                              appSettings->translationsPath());
    }

    app.installTranslator(&qtBaseTranslator);

    QTranslator appTranslator;
    ok = appTranslator.load
         (
             QString("ghostwriter_") + appSettings->locale(),
             appSettings->translationsPath()
         );

    if (!ok) {
        appTranslator.load
        (
            "ghostwriter_en",
            appSettings->translationsPath()
        );
    }

    app.installTranslator(&appTranslator);

    QString filePath = QString();

    if (argc > 1) {
        filePath = app.arguments().at(1);
    }

    ghostwriter::MainWindow window(filePath);

    window.show();
    return app.exec();
}
