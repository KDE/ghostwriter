/*
 * SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QKeyCombination>
#include <QKeySequence>
#include <QMap>
#include <Qt>

#include "appactions.h"

namespace ghostwriter
{
const QString NO_ICON = QString();
const QString UNASSIGNED = QString();

class AppActionsPrivate
{
public:
    AppActionsPrivate()
    {
    }
    ~AppActionsPrivate()
    {
    }

    QAction *addAction(AppActions::ActionType actionType,
                       KStandardAction::StandardAction standardType,
                       const QString &icon = NO_ICON,
                       Qt::ShortcutContext context = Qt::WindowShortcut);

    QAction *addCheckAction(AppActions::ActionType actionType,
                            KStandardAction::StandardAction standardType,
                            const QString &icon = NO_ICON,
                            Qt::ShortcutContext context = Qt::WindowShortcut);

    QAction *addAction(AppActions::ActionType actionType,
                       const QString &actionName,
                       const QString &text,
                       const QString &icon = NO_ICON,
                       const QKeySequence &defaultShortcut = UNASSIGNED,
                       Qt::ShortcutContext context = Qt::WindowShortcut);

    QAction *addCheckAction(AppActions::ActionType actionType,
                            const QString &actionName,
                            const QString &text,
                            const QString &icon = NO_ICON,
                            const QKeySequence &defaultShortcut = UNASSIGNED,
                            Qt::ShortcutContext context = Qt::WindowShortcut);

    KActionCollection *collection;
    SvgIconTheme *iconTheme;

    QMap<AppActions::ActionType, QString> actionNames;
    QMap<AppActions::ActionType, QAction *> actionsByType;
    QMap<QString, QAction *> actionsByName;
};

AppActions::AppActions(KActionCollection *collection, SvgIconTheme *iconTheme, QObject *parent)
    : QObject(parent)
    , d(new AppActionsPrivate())
{
    d->collection = collection;
    d->iconTheme = iconTheme;

    QAction *action;

    // File Menu Actions

    d->addAction(New, KStandardAction::New, "new-file");
    d->addAction(Open, KStandardAction::Open, "open-file");
    d->addAction(OpenRecent, KStandardAction::OpenRecent, "open-recent");
    d->addAction(ReopenLastClosed, "file_reopen_last_closed", tr("Reopen Last Closed File"), NO_ICON, tr("Ctrl+Shift+T"));

    for (int i = OpenMostRecent; i <= OpenLeastRecent; i++) {
        char actionName[26];

        sprintf(actionName, "file_open_recent_%03d", i);
        action = d->addAction((ActionType)i, actionName, "");
    }

    action = get(ReopenLastClosed);
    action->setEnabled(false);
    QObject::connect(action, &QAction::triggered, get(OpenMostRecent), &QAction::trigger);

    d->addAction(ClearRecentFilesList, "file_clear_recent_files", tr("Clear List"), "clear");
    d->addAction(Save, KStandardAction::Save, "save");
    d->addAction(SaveAs, KStandardAction::SaveAs, "save-as");
    d->addAction(RenameFile, KStandardAction::RenameFile, "rename-file");
    d->addAction(Reload, "file_reload", tr("Reload from Disk..."), "reload", QKeySequence::Refresh);
    d->addAction(Export, "file_export", tr("Export"), "export", tr("CTRL+E"));
    d->addAction(Quit, KStandardAction::Quit, "close");

    // Edit Menu Actions

    d->addAction(Undo, KStandardAction::Undo, "undo");
    d->addAction(Redo, KStandardAction::Redo, "redo");
    d->addAction(Cut, KStandardAction::Cut, "cut");
    d->addAction(Copy, KStandardAction::Copy, "copy");
    d->addAction(Paste, KStandardAction::Paste, "paste");
    d->addAction(CopyHTML, "edit_copy_html", tr("Copy &HTML"), NO_ICON, tr("SHIFT+CTRL+C"));
    d->addAction(SelectAll, KStandardAction::SelectAll, "select-all");
    d->addAction(Deselect, KStandardAction::Deselect, "deselect");
    d->addAction(InsertImage, "edit_insert_image", tr("&Insert Image..."), "insert-image");
    d->addAction(Find, KStandardAction::Find, "find");
    d->addAction(Replace, KStandardAction::Replace, "find-replace");
    d->addAction(FindNext, KStandardAction::FindNext);
    d->addAction(FindPrev, KStandardAction::FindPrev);
    d->addAction(Spelling, KStandardAction::Spelling, "spell-check");

    // Format Menu Actions

    d->addAction(Strong, "format_strong", tr("Strong"), "strong", QKeySequence::Bold);
    d->addAction(Emphasis, "format_emphasis", tr("Emphasis"), "emphasis", QKeySequence::Italic);
    d->addAction(Strikethrough, "format_strikethrough", tr("Strikethrough"), "strikethrough", tr("CTRL+K"));
    d->addAction(InsertHTMLComment, "format_insert_html_comment", tr("HTML Comment"), "comment", tr("CTRL+/"));
    d->addAction(IndentText, "format_indent", tr("Indent"), "indent", tr("Tab"));
    d->addAction(UnindentText, "format_unindent", tr("Unindent"), "unindent", tr("SHIFT+Tab"));
    d->addAction(CodeFences, "format_code_fences", tr("&Code Fences"), "code-block", tr("CTRL+G"));

    action = d->addAction(BlockQuote, "format_block_quote", tr("Block Quote"), "block-quote", tr("CTRL+SHIFT+."));
    action->setToolTip(tr("Formats the current line or selected lines as a block quote."));
    action->setWhatsThis(
        tr("Formats the current line or selected lines as a block quote, or "
           "if already a block quote, as a nested block quote."));

    action = d->addAction(StripBlockQuote, "format_strip_block_quote", tr("Strip Block Quote"), NO_ICON, tr("CTRL+SHIFT+,"));
    action->setToolTip(tr("Strips the current line or selected lines of block quote formatting."));
    action->setWhatsThis(
        tr("Strips the current line or selected lines of block quote "
           "formatting. If the block is nested, only the outermost layer of "
           "block quote formatting will be stripped."));

    d->addAction(BulletListAsterisk, "format_bullet_list_asterisk", tr("* Bullet List"), "unordered-list", tr("CTRL+8"));
    action->setToolTip(
        tr("Formats the current line or selected lines as a "
           "bullet point list with an asterisk (*)."));

    d->addAction(BulletListMinus, "format_bullet_list_minus", tr("- Bullet List"), "unordered-list", tr("CTRL+-"));
    action->setToolTip(
        tr("Formats the current line or selected lines as a "
           "bullet point list with a minus sign (-)."));

    action = d->addAction(BulletListPlus, "format_bullet_list_plus", tr("+ Bullet List"), "unordered-list", tr("CTRL++"));
    action->setToolTip(
        tr("Formats the current line or selected lines as a "
           "bullet point list with a plus sign (+)."));

    d->addAction(NumberedListPeriod, "format_numbered_list_period", tr("1. Numbered List"), "ordered-list", tr("CTRL+1"));
    action->setToolTip(
        tr("Formats the current line or selected lines as a "
           "numbered list."));
    action->setWhatsThis(
        tr("<p>Formats the current line or selected lines as a numbered list.</p>"
           "<p>Example:</p>"
           "<p>"
           "1. item one<br/>"
           "2. item two<br/>"
           "3. item three<br/>"
           "</p>"));

    action = d->addAction(NumberedListParenthesis, "format_numbered_list_parenthesis", tr("1) Numbered List"), "ordered-list", tr("CTRL+0"));
    action->setToolTip(
        tr("Formats the current line or selected lines as a "
           "numbered list."));
    action->setWhatsThis(
        tr("<p>Formats the current line or selected lines as a numbered list.</p>"
           "<p>Example:</p>"
           "<p>"
           "1) item one<br/>"
           "2) item two<br/>"
           "3) item three<br/>"
           "</p>"
           "<p>Note that this numbering format is not supported by all "
           "Markdown processors and is not part of CommonMark.</p>"));

    action = d->addAction(TaskList, "format_task_list", tr("Task List"), "task-list", tr("CTRL+T"));
    action->setToolTip(tr("Formats the current line or selected lines as a task list."));
    action->setWhatsThis(
        tr("<p>Formats the current line or selected lines as a task list.</p>"
           "<p>Example:</p>"
           "<p>"
           "- [ ] item one<br/>"
           "- [ ] item two<br/>"
           "- [ ] item three<br/>"
           "</p>"
           "<p>Note that task lists are not supported by all Markdown "
           "processors and are not part of CommonMark.</p>"));

    action = d->addAction(TaskComplete, "format_toggle_task_complete", tr("Toggle Task(s) Complete"), "task-complete", tr("CTRL+D"), Qt::WindowShortcut);
    action->setToolTip(
        tr("Toggle the current task item or selected task items "
           "as complete or incomplete."));
    action->setWhatsThis(
        tr("<p>Toggle the state of the current task item or selected task "
           "items from a task list as complete or incomplete.</p>"
           "<p>Applying this action to an incomplete task item in a task list "
           "will result in placing an 'x' between its brackets, like so:</p>"
           "<p>"
           "- [x] item one<br/>"
           "</p>"
           "<p>Likewise, applying this action to an completed task item in a "
           "task list will result in removing its 'x' between its brackets, "
           "like so:</p>"
           "<p>"
           "- [ ] item one<br/>"
           "</p>"
           "<p>Note that task lists are not supported by all Markdown "
           "processors and are not part of CommonMark.</p>"));

    // View Menu Actions

    d->addCheckAction(FullScreen, KStandardAction::FullScreen, "full-screen");

    d->addCheckAction(DistractionFreeMode, "view_distraction_free_mode", tr("Distraction-Free Mode"), "distraction-free-mode", tr("SHIFT+F11"));

    d->addCheckAction(Preview, "view_preview", tr("Live Preview"), "live-preview", tr("CTRL+P"));

    action = d->addCheckAction(HemingwayMode, "view_hemingway_mode", tr("Hemingway Mode"), "hemingway-mode", tr("SHIFT+Backspace"));
    action->setToolTip(tr("Toggles Hemingway mode."));
    action->setWhatsThis(
        tr("Hemingway mode disables the backspace and delete keys to help you "
           "resist the temptation to edit your document as you write."));

    action = d->addCheckAction(DarkMode, "view_dark_mode", tr("Dark Mode"), "dark-mode");
    action->setToolTip(tr("Enables/disables the current theme's dark color scheme."));
    action->setWhatsThis(
        tr("<p>Enables/disables the current theme's dark color scheme.</p>"
           "<p>If the current theme does not support a dark "
           "color scheme, then this option will have no effect.</p>"));

    d->addCheckAction(ShowSidebar, "view_sidebar", tr("Show Sidebar"), "show-sidebar", tr("CTRL+Space"));

    action = d->addAction(ShowOutline, "view_outline", tr("Outline"), "outline", tr("CTRL+J"));
    action->setToolTip(tr("Shows the document outline."));
    action->setWhatsThis(
        tr("<p>Shows the document outline.</p>"
           "<p>Use the mouse plus left click on a heading in the document, "
           "or else the up and down arrows on the keyboard plus the ENTER key, "
           "to jump to different locations within the current document.</p>"));

    d->addAction(ShowSessionStatistics, "view_session_statistics", tr("Session Statistics"), "session-statistics");

    d->addAction(ShowDocumentStatistics, "view_document_statistics", tr("Document Statistics"), "document-statistics");

    action = d->addAction(ShowCheatSheet, "view_cheat_sheet", tr("Cheat Sheet"), "cheat-sheet");
    action->setWhatsThis(tr("Shows a quick reference guide for Markdown in the sidebar."));

    d->addAction(ZoomIn, KStandardAction::ZoomIn, "zoom-in");
    d->addAction(ZoomOut, KStandardAction::ZoomOut, "zoom-out");

    // Settings Menu Actions

    d->addAction(ChangeTheme, "settings_theme", tr("Theme..."), "theme");
    d->addAction(ChangeFont, "settings_font", tr("Font..."), "font");
    d->addAction(SwitchApplicationLanguage, KStandardAction::SwitchApplicationLanguage, "language");
    d->addAction(PreviewOptions, "settings_preview_options", tr("Preview Options..."), "settings");

    action = d->addAction(Preferences, KStandardAction::Preferences, "preferences");
    action->setMenuRole(QAction::PreferencesRole);

    // Help Menu Actions

    d->addAction(HelpContents, KStandardAction::HelpContents, "documentation");
    d->addAction(WhatsThis, KStandardAction::WhatsThis, "whats-this");
    d->addAction(ReportBug, KStandardAction::ReportBug, "bug-report");
    d->addAction(Donate, KStandardAction::Donate, "donate");

    action = d->addAction(AboutApp, KStandardAction::AboutApp);
    action->setIcon(QIcon(":/resources/icons/sc-apps-ghostwriter.svg"));
    action->setMenuRole(QAction::AboutRole);

    action = d->addAction(AboutKDE, KStandardAction::AboutKDE);
    action->setIcon(QIcon(":/resources/icons/kdeapp.svg"));
}

AppActions::~AppActions()
{
    ;
}

QString AppActions::name(ActionType actionType) const
{
    return d->actionNames.value(actionType, QString());
}

KActionCollection *AppActions::collection() const
{
    return d->collection;
}

QAction *AppActions::get(ActionType actionType) const
{
    return d->actionsByType.value(actionType, nullptr);
}

QAction *AppActions::get(const QString &actionName) const
{
    return d->actionsByName.value(actionName, nullptr);
}

QStringList AppActions::allActionNames() const
{
    return d->actionsByName.keys();
}

QAction *
AppActionsPrivate::addAction(AppActions::ActionType actionType, KStandardAction::StandardAction standardId, const QString &icon, Qt::ShortcutContext context)
{
    auto action = collection->addAction(standardId);
    auto name = KStandardAction::name(standardId);

    actionNames.insert(actionType, name);
    actionsByType.insert(actionType, action);
    actionsByName.insert(name, action);
    action->setShortcutContext(context);

    if (icon.isEmpty()) {
        action->setIcon(QIcon());
    } else {
        action->setIcon(iconTheme->icon(icon));
    }

    return action;
}

QAction *AppActionsPrivate::addCheckAction(AppActions::ActionType actionType,
                                           KStandardAction::StandardAction standardId,
                                           const QString &icon,
                                           Qt::ShortcutContext context)
{
    auto action = addAction(actionType, standardId, icon, context);
    action->setCheckable(true);
    action->setChecked(false);

    if (icon.isEmpty()) {
        action->setIcon(iconTheme->checkableIcon("checkbox-unchecked", "checkbox-checked"));
    }

    return action;
}

QAction *AppActionsPrivate::addAction(AppActions::ActionType actionType,
                                      const QString &actionName,
                                      const QString &text,
                                      const QString &icon,
                                      const QKeySequence &defaultShortcut,
                                      Qt::ShortcutContext context)
{
    auto action = collection->addAction(actionName);

    actionNames.insert(actionType, actionName);
    actionsByType.insert(actionType, action);
    actionsByName.insert(actionName, action);
    action->setText(text);
    action->setShortcutContext(context);

    if (!icon.isEmpty()) {
        action->setIcon(iconTheme->icon(icon));
    }

    collection->setDefaultShortcut(action, defaultShortcut);
    return action;
}

QAction *AppActionsPrivate::addCheckAction(AppActions::ActionType actionType,
                                           const QString &actionName,
                                           const QString &text,
                                           const QString &icon,
                                           const QKeySequence &defaultShortcut,
                                           Qt::ShortcutContext context)
{
    auto action = addAction(actionType, actionName, text, icon, defaultShortcut, context);
    action->setCheckable(true);
    action->setChecked(false);

    if (icon.isEmpty()) {
        action->setIcon(iconTheme->checkableIcon("checkbox-unchecked", "checkbox-checked"));
    }

    return action;
}

} // namespace ghostwriter