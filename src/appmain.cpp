/***********************************************************************
 *
 * Copyright (C) 2014-2021 wereturtle
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
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include <QDebug>

#include "mainwindow.h"
#include "appsettings.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

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
    QLocale::setDefault(QLocale(appSettings->locale()));

    QStringList baseTranslators = { "qt", "qtbase", "ghostwriter" };

    for (auto translatorStr : baseTranslators) {
        QTranslator translator;
        bool ok = false;

        if (translatorStr != "ghostwriter") {
            const QString& translation_loc = 
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#else
                QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#endif
            ok = translator.load(translatorStr + "_" + appSettings->locale(),
                                        translation_loc);        
        }
        if (!ok) {
            ok = translator.load(translatorStr + "_" + appSettings->locale(),
                              appSettings->translationsPath());

            if (!ok && (QString("ghostwriter") == translatorStr)) {
                ok = translator.load("ghostwriter_en", appSettings->translationsPath());
            }
        }

        if (ok) {
            app.installTranslator(&translator);
        }
    }

    QString filePath = QString();

    if (argc > 1) {
        filePath = app.arguments().at(1);
    }

    ghostwriter::MainWindow window(filePath);

    window.show();
    return app.exec();
}
