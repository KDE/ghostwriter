/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QObject>
#include <QDialog>
#include <QVariant>
#include <QMap>

class AppSettings;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        PreferencesDialog(QWidget* parent = NULL);

        /**
         * Destructor.
         */
        virtual ~PreferencesDialog();

    private slots:
        void onFocusModeChanged(int index);
        void onEditorWidthChanged(int index);
        void onInterfaceStyleChanged(int index);
        void onBlockquoteStyleChanged(int index);
        void onEmphasisStyleChanged(int index);
        void onDictionaryChanged(int index);
        void onHudWindowButtonLayoutChanged(int index);
        void showAutoMatchFilterDialog();

    private:
        AppSettings* appSettings;

        QWidget* initializeGeneralTab();
        QWidget* initializeEditorTab();
        QWidget* initializeSpellCheckTab();
        QWidget* initializeHudTab();

        QString languageName(const QString& language);
};

#endif // PREFERENCESDIALOG_H
