/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

    // Disable icons in menus for now, since matching their colors to the
    // current theme is not supported yet.
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);

    QApplication app(argc, argv);

#if QT_VERSION >= 0x050700 && defined(Q_OS_LINUX)
    QGuiApplication::setDesktopFileName("ghostwriter");
#endif

    // Call this to force settings initialization before the application
    // fully launches.
    //
    ghostwriter::AppSettings::instance();
//     QLocale::setDefault(QLocale(appSettings->locale()));

//     QStringList baseTranslators = { "qt", "qtbase", "ghostwriter" };

//     for (auto translatorStr : baseTranslators) {
//         QTranslator *translator = new QTranslator(&app);
//         QString fileName = translatorStr + "_" + appSettings->locale();
//         bool ok = false;

//         if (translatorStr != "ghostwriter") {
//             const QString& translationLocation = 
// #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//                 QLibraryInfo::location(QLibraryInfo::TranslationsPath);
// #else
//                 QLibraryInfo::path(QLibraryInfo::TranslationsPath);
// #endif
//             ok = translator->load(fileName, translationLocation);        
//         } else {
//             ok = translator->load(fileName, appSettings->translationsPath());
//         }

//         if (ok) {
//             app.installTranslator(translator);
//         }
//     }

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
