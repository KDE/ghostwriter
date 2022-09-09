/***********************************************************************
 *
 * Copyright (C) 2014-2022 wereturtle
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
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QWindow>

#include <QDebug>

#include "mainwindow.h"
#include "appsettings.h"

int main(int argc, char *argv[])
{
    bool disableGPU = false;

    // Unfortunately, we must preparse the arguments for the --disable-gpu
    // option rather than using QCommandLineParser since we must set the
    // software rendering attribute before creating the QApplication.
    //
    for (int i = 0; i < argc; i++) {
        if (0 == strcmp(argv[i], "--disable-gpu")) {
            disableGPU = true;
            break;
        }
    }

    if (disableGPU) {
        QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if defined(Q_OS_WIN)
    // For Qt 5, use ANGLE instead of OpenGL to bypass bug where full screen
    // windows under Windows 10 and OpenGL will not show menus from the menu
    // bar (or any other popup menus).  For Qt 6, this is option is no longer
    // available, so allow the user to pass in the software OpenGL option if
    // desired. (Note: Software rendering can be buggy, so leave it optional).
    // Sadly, the full screen OpenGL workaround in Qt's documentation does not
    // actually work.
    //
    // Thank you, Microsoft (and now Qt for removing ANGLE), you made my day.
    //
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!disableGPU) {
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES, true);
    }
#endif
#endif

    QApplication app(argc, argv);

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

    QCommandLineParser clParser;
    clParser.setApplicationDescription(QCoreApplication::translate("main",
        "Welcome to ghostwriter!"));
    clParser.addHelpOption();
    clParser.addVersionOption();
    clParser.addPositionalArgument("file",
        QCoreApplication::translate("main", "(Optional) File to open."));

    QCommandLineOption renderingOption("disable-gpu",
        QCoreApplication::translate("main", "Disables GPU acceleration."));

    clParser.addOption(renderingOption);
    clParser.process(app);

    QStringList posArgs = clParser.positionalArguments();

    if (posArgs.size() > 0) {
        filePath = posArgs.first();
    }

    // Note: --disable-gpu option was already processed. We added it here
    //       only so it is displayed in the help output.

    ghostwriter::MainWindow window(filePath);

    window.show();
    return app.exec();
}
