/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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
#include <QWidget>
#include <QSettings>
#include <QDir>
#include <QDebug>

#include "MainWindow.h"
#include "AppSettings.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Call this to force settings initialization before the application
    // fully launches.
    //
    AppSettings::getInstance();

    QString filePath = QString();

    if (argc > 1)
    {
        filePath = app.arguments().at(1);
    }

    MainWindow window(filePath);

    window.show();
    return app.exec();
}
