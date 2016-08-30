#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QObject>
#include <QDialog>

class PreferencesDialog : public QDialog
{
    public:
        /**
         * Constructor.
         */
        PreferencesDialog(QWidget* parent = NULL);

        /**
         * Destructor.
         */
        ~PreferencesDialog();
};

#endif // PREFERENCESDIALOG_H
