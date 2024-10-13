/*
 * SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef APPACTIONS_H
#define APPACTIONS_H

#include <QAction>
#include <QDebug>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <Qt>

#include <KActionCollection>
#include <KStandardAction>

#include "theme/svgicontheme.h"

namespace ghostwriter
{
class AppActionsPrivate;

/**
 * Handles QAction() creation amd signal/slot registry for all the application's
 * actions.
 */
class AppActions : QObject
{
    Q_OBJECT
public:
    /**
     *  IDs for all the application's actions.
     */
    typedef enum ActionType {
        ActionNone,
        // File Menu
        New,
        Open,
        OpenRecent,
        ReopenLastClosed,
        OpenMostRecent,
        OpenRecent001 = OpenMostRecent,
        OpenRecent002,
        OpenRecent003,
        OpenRecent004,
        OpenRecent005,
        OpenRecent006,
        OpenRecent007,
        OpenRecent008,
        OpenRecent009,
        OpenRecent010,
        OpenLeastRecent = OpenRecent010,
        ClearRecentFilesList,
        Save,
        SaveAs,
        RenameFile,
        Reload,
        Export,
        Quit,
        // Edit Menu
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        CopyHTML,
        SelectAll,
        Deselect,
        InsertImage,
        Find,
        FindNext,
        FindPrev,
        Replace,
        Spelling,
        // Format Menu
        Emphasis,
        Strong,
        Strikethrough,
        InsertHTMLComment,
        IndentText,
        UnindentText,
        CodeFences,
        BlockQuote,
        StripBlockQuote,
        BulletListAsterisk,
        BulletListMinus,
        BulletListPlus,
        NumberedListPeriod,
        NumberedListParenthesis,
        TaskList,
        TaskComplete,
        // View Menu
        FullScreen,
        DistractionFreeMode,
        Preview,
        HemingwayMode,
        DarkMode,
        ShowMenubar,
        ShowStatusbar,
        ShowSidebar,
        ShowOutline,
        ShowSessionStatistics,
        ShowDocumentStatistics,
        ShowCheatSheet,
        ZoomIn,
        ZoomOut,
        // Settings Menu
        ChangeTheme,
        ChangeFont,
        SwitchApplicationLanguage,
        PreviewOptions,
        Preferences,
        // Help Menu
        HelpContents,
        WhatsThis,
        ReportBug,
        Donate,
        AboutApp,
        AboutKDE,

        ActionCount
    } ActionType;

    /**
     * Constructor that takes a KActionCollection and the SVG icon theme
     * to use for loading action icons.
     */
    AppActions(KActionCollection *collection, SvgIconTheme *iconTheme, QObject *parent = nullptr);

    /**
     * Destructor.
     */
    ~AppActions();

    /**
     * Returns a list of all action names used by this application.
     */
    QStringList allActionNames() const;

    /**
     * Returns the collection associated with this object.
     */
    KActionCollection *collection() const;

    /**
     * Connects the given receiver object / method slot to the action specified
     * by the given name.
     */
    template<class Receiver, class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, void>::type
    connect(const QString &actionName, const Receiver *receiver, Func slot, Qt::ConnectionType atype = Qt::AutoConnection)
    {
        auto action = get(actionName);

        if (nullptr != action) {
            QObject::connect(action, &QAction::triggered, receiver, slot, atype);
        } else {
            qCritical() << "AppActions::connect(): Unknown action name specified:" << actionName;
        }
    }

    /**
     * Connects the given function slot to the action specified
     * by the given name.
     */
    template<class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, void>::type
    connect(const QString &actionName, Func slot, Qt::ConnectionType atype = Qt::AutoConnection)
    {
        auto action = get(actionName);

        if (nullptr != action) {
            QObject::connect(action, &QAction::triggered, slot, atype);
        } else {
            qCritical() << "AppActions::connect(): Unknown action name specified:" << actionName;
        }
    }

    /**
     * Connects the given receiver object / method slot to the action specified
     * by the given type identifier.
     */
    template<class Receiver, class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, void>::type
    connect(ActionType actionType, const Receiver *receiver, Func slot, Qt::ConnectionType atype = Qt::AutoConnection)
    {
        auto action = get(actionType);

        if (nullptr != action) {
            QObject::connect(action, &QAction::triggered, receiver, slot, atype);
        } else {
            qCritical() << "AppActions::connect(): Unknown action type specified:" << actionType;
        }
    }

    /**
     * Connects the given function slot to the action specified
     * by the given type identifier.
     */
    template<class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, void>::type
    connect(ActionType actionType, Func slot, Qt::ConnectionType atype = Qt::AutoConnection)
    {
        auto action = get(actionType);

        if (nullptr != action) {
            QObject::connect(action, &QAction::triggered, slot, atype);
        } else {
            qCritical() << "AppActions::connect(): Unknown action type specified:" << actionType;
        }
    }

    /**
     * Returns the QAction for the given type identifier.
     */
    QAction *get(ActionType actionType) const;

    /**
     * Returns the QAction for the given action name.
     */
    QAction *get(const QString &actionName) const;

    /**
     * Returns the name of the given action type identifier.
     */
    QString name(ActionType actionType) const;

private:
    QScopedPointer<AppActionsPrivate> d;
};
} // namespace ghostwriter

#endif // APPACTIONS_H
