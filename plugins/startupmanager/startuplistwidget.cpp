#include "startuplistwidget.h"
#include "startupitem.h"

#include <QDebug>
#include <QLabel>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QDirIterator>

#include <fstream>
#include <qdiriterator.h>
#include <sstream>
#include <stdio.h>
#include <string>


#include <glib-object.h>
#include <glib.h>


static char **
gsm_util_get_standard_autostart_dirs (void)
{
        GPtrArray          *dirs;
        const char * const *system_config_dirs;
        const char * const *system_data_dirs;
        int                 i;

        dirs = g_ptr_array_new ();

        g_ptr_array_add (dirs,
                         g_build_filename (g_get_user_config_dir (),
                                           "autostart", NULL));

        system_data_dirs = g_get_system_data_dirs ();
        for (i = 0; system_data_dirs[i]; i++) {
                g_ptr_array_add (dirs,
                                 g_build_filename (system_data_dirs[i],
                                                   "gnome", "autostart", NULL));
        }

        system_config_dirs = g_get_system_config_dirs ();
        for (i = 0; system_config_dirs[i]; i++) {
                g_ptr_array_add (dirs,
                                 g_build_filename (system_config_dirs[i],
                                                   "autostart", NULL));
        }

        g_ptr_array_add (dirs, NULL);

        return (char **) g_ptr_array_free (dirs, FALSE);
}

char **
gsm_util_get_autostart_dirs ()
{
//        if (autostart_dirs) {
//                return g_strdupv ((char **)autostart_dirs);
//        }

        return gsm_util_get_standard_autostart_dirs ();
}

//static int
//gsp_app_manager_get_dir_index (GspAppManager *manager,
//                               const char    *dir)
//{
//        GSList    *l;
//        GspXdgDir *xdgdir;

//        g_return_val_if_fail (GSP_IS_APP_MANAGER (manager), -1);
//        g_return_val_if_fail (dir != NULL, -1);

//        for (l = manager->priv->dirs; l != NULL; l = l->next) {
//                xdgdir = l->data;
//                if (strcmp (dir, xdgdir->dir) == 0) {
//                        return xdgdir->index;
//                }
//        }

//        return -1;
//}


//const char *
//gsp_app_manager_get_dir (GspAppManager *manager,
//                         unsigned int   index)
//{
//        GSList    *l;
//        GspXdgDir *xdgdir;

//        g_return_val_if_fail (GSP_IS_APP_MANAGER (manager), NULL);

//        for (l = manager->priv->dirs; l != NULL; l = l->next) {
//                xdgdir = l->data;
//                if (index == xdgdir->index) {
//                        return xdgdir->dir;
//                }
//        }

//        return NULL;
//}

//static gboolean
//gsp_app_manager_xdg_dir_monitor (GFileMonitor      *monitor,
//                                 GFile             *child,
//                                 GFile             *other_file,
//                                 GFileMonitorEvent  flags,
//                                 gpointer           data)
//{
//        GspAppManager *manager;
//        GspApp        *old_app;
//        GspApp        *app;
//        GFile         *parent;
//        char          *basename;
//        char          *dir;
//        char          *path;
//        int            index;

//        manager = GSP_APP_MANAGER (data);

//        basename = g_file_get_basename (child);
//        if (!g_str_has_suffix (basename, ".desktop")) {
//                /* not a desktop file, we can ignore */
//                g_free (basename);
//                return TRUE;
//        }
//        old_app = gsp_app_manager_find_app_with_basename (manager, basename);

//        parent = g_file_get_parent (child);
//        dir = g_file_get_path (parent);
//        g_object_unref (parent);

//        index = gsp_app_manager_get_dir_index (manager, dir);
//        if (index < 0) {
//                /* not a directory we know; should never happen, though */
//                g_free (dir);
//                return TRUE;
//        }

//        path = g_file_get_path (child);

//        switch (flags) {
//        case G_FILE_MONITOR_EVENT_CHANGED:
//        case G_FILE_MONITOR_EVENT_CREATED:
//                /* we just do as if it was a new file: GspApp is clever enough
//                 * to do the right thing */
//                app = gsp_app_new (path, (unsigned int) index);

//                /* we didn't have this app before, so add it */
//                if (old_app == NULL && app != NULL) {
//                        gsp_app_manager_add (manager, app);
//                        g_object_unref (app);
//                }
//                /* else: it was just updated, GspApp took care of
//                 * sending the event */
//                break;
//        case G_FILE_MONITOR_EVENT_DELETED:
//                if (!old_app) {
//                        /* it got deleted, but we don't know about it, so
//                         * nothing to do */
//                        break;
//                }

//                _gsp_app_manager_handle_delete (manager, old_app,
//                                                basename, index);
//                break;
//        default:
//                break;
//        }

//        g_free (path);
//        g_free (dir);
//        g_free (basename);

//        return TRUE;
//}

//static void
//_gsp_app_manager_fill_from_dir (GspAppManager *manager,
//                                GspXdgDir     *xdgdir)
//{
//        GFile      *file;
//        GDir       *dir;
//        const char *name;

//        file = g_file_new_for_path (xdgdir->dir);
//        xdgdir->monitor = g_file_monitor_directory (file, G_FILE_MONITOR_NONE,
//                                                    NULL, NULL);
//        g_object_unref (file);

//        if (xdgdir->monitor) {
//                g_signal_connect (xdgdir->monitor, "changed",
//                                  G_CALLBACK (gsp_app_manager_xdg_dir_monitor),
//                                  manager);
//        }

//        dir = g_dir_open (xdgdir->dir, 0, NULL);
//        if (!dir) {
//                return;
//        }

//        while ((name = g_dir_read_name (dir))) {
//                GspApp *app;
//                char   *desktop_file_path;

//                if (!g_str_has_suffix (name, ".desktop")) {
//                        continue;
//                }

//                desktop_file_path = g_build_filename (xdgdir->dir, name, NULL);
//                app = gsp_app_new (desktop_file_path, xdgdir->index);

//                if (app != NULL) {
//                        gsp_app_manager_add (manager, app);
//                        g_object_unref (app);
//                }

//                g_free (desktop_file_path);
//        }

//        g_dir_close (dir);
//}
void test_func()
{
    char **autostart_dirs;
    int    i;

    autostart_dirs = gsm_util_get_autostart_dirs ();
    /* we always assume that the first directory is the user one */
    g_assert (g_str_has_prefix (autostart_dirs[0],
                                g_get_user_config_dir ()));

    for (i = 0; autostart_dirs[i] != NULL; i++) {
//            GspXdgDir *xdgdir;

//            if (gsp_app_manager_get_dir_index (manager,
//                                               autostart_dirs[i]) >= 0) {
//                    continue;
//            }
//            xdgdir = _gsp_xdg_dir_new (autostart_dirs[i], i);
//            manager->priv->dirs = g_slist_prepend (manager->priv->dirs,
//                                                   xdgdir);
//            _gsp_app_manager_fill_from_dir (manager, xdgdir);
    }

    g_strfreev (autostart_dirs);
}

//static void
//_fill_iter_from_app (GtkListStore *list_store,
//                     GtkTreeIter  *iter,
//                     GspApp       *app)
//{
//        gboolean    hidden;
//        gboolean    display;
//        gboolean    enabled;
//        gboolean    shown;
//        GIcon      *icon;
//        const char *description;
//        const char *app_name;

//        hidden      = gsp_app_get_hidden (app);
//        display     = gsp_app_get_display (app);
//        enabled     = gsp_app_get_enabled (app);
//        shown       = gsp_app_get_shown (app);
//        icon        = gsp_app_get_icon (app);
//        description = gsp_app_get_description (app);
//        app_name    = gsp_app_get_name (app);

//        if (G_IS_THEMED_ICON (icon)) {
//                GtkIconTheme       *theme;
//                const char * const *icon_names;

//                theme = gtk_icon_theme_get_default ();
//                icon_names = g_themed_icon_get_names (G_THEMED_ICON (icon));
//                if (icon_names[0] == NULL ||
//                    !gtk_icon_theme_has_icon (theme, icon_names[0])) {
//                        g_object_unref (icon);
//                        icon = NULL;
//                }
//        } else if (G_IS_FILE_ICON (icon)) {
//                GFile *iconfile;

//                iconfile = g_file_icon_get_file (G_FILE_ICON (icon));
//                if (!g_file_query_exists (iconfile, NULL)) {
//                        g_object_unref (icon);
//                        icon = NULL;
//                }
//        }

//        if (icon == NULL) {
//                icon = g_themed_icon_new (STARTUP_APP_ICON);
//        }

//        gtk_list_store_set (list_store, iter,
//                            STORE_COL_VISIBLE, !hidden && shown && display,
//                            STORE_COL_ENABLED, enabled,
//                            STORE_COL_GICON, icon,
//                            STORE_COL_DESCRIPTION, description,
//                            STORE_COL_APP, app,
//                            STORE_COL_SEARCH, app_name,
//                            -1);
//        g_object_unref (icon);
//}
//static void
//append_app (GsmPropertiesDialog *dialog,
//            GspApp              *app)
//{
//        GtkTreeIter   iter;

//        gtk_list_store_append (dialog->priv->list_store, &iter);
//        _fill_iter_from_app (dialog->priv->list_store, &iter, app);

//        g_signal_connect_swapped (app, "changed",
//                                  G_CALLBACK (_app_changed), dialog);
//}
//static void
//populate_model (GsmPropertiesDialog *dialog)
//{
//        GSList *apps;
//        GSList *l;

//        apps = gsp_app_manager_get_apps (dialog->priv->manager);
//        for (l = apps; l != NULL; l = l->next) {
//                append_app (dialog, GSP_APP (l->data));
//        }
//        g_slist_free (apps);
//}

//populate_model (dialog);

//inline QPixmap getDesktopFileIcon(QString desktopFile, int iconSize)
//{
//    std::string desktop_file;
//    desktopFile = desktopFile.toStdString();
//    std::ifstream in;
//    in.open(desktop_file);
//    QIcon defaultExecutableIcon = QIcon::fromTheme("application-x-executable");
//    QIcon icon;
//    QString iconName;
//    while(!in.eof()) {
//        std::string line;
//        std::getline(in,line);
//        iconName = QString::fromStdString(line);

//        if (iconName.startsWith("Icon=")) {
//            iconName.remove(0,5); // remove the first 5 chars
//        } else {
//            continue;
//        }

//        if (iconName.contains("/")) {
//            icon = QIcon(iconName);
//        } else {
//            icon = QIcon::fromTheme(iconName, defaultExecutableIcon);
//            break;
//        }
//    }
//    in.close();

//    qreal devicePixelRatio = qApp->devicePixelRatio();

//    QPixmap pixmap = icon.pixmap(iconSize * devicePixelRatio, iconSize * devicePixelRatio);
//    pixmap.setDevicePixelRatio(devicePixelRatio);

//    return pixmap;
//}

inline QString getDesktopFile(QString processname)
{
    QDirIterator dir("/usr/share/applications", QDirIterator::Subdirectories);
//    std::string desktopFile;
    QString desktopFile;

    QString procname = processname.toLower();
    procname.replace("_", "-");

    QString processFilename = procname + ".desktop";

    while(dir.hasNext()) {
        if (dir.fileInfo().suffix() == "desktop") {
            if (dir.fileName().toLower().contains(processFilename)) {
//                desktopFile = dir.filePath().toStdString();
                desktopFile = dir.filePath();
                break;
            }
        }
        dir.next();
    }

//    return QString::fromStdString(desktopFile);
    return desktopFile;
}

//inline QString getDisplayNameFromName(QString procName, QString desktopFile, bool displayProcessName)
//{
//    QString procname = procName.toLower();
//    if (processDescriptions.contains(procname)) {
//        if (displayProcessName) {
//            return QString("%1    ( %2 )").arg(processDescriptions[procname], procName);
//        } else {
//            return processDescriptions[procname];
//        }
//    }

//    if (desktopFile.size() == 0) {
//        return procName;
//    }

//    std::ifstream in;
//    in.open(desktopFile);
//    QString displayName = procName;
//    while(!in.eof()) {
//        std::string line;
//        std::getline(in,line);

//        QString lineContent = QString::fromStdString(line);

//        QString localNameFlag = QString("Name[%1]=").arg(QLocale::system().name());
//        QString nameFlag = "Name=";
//        QString genericNameFlag = QString("GenericName[%1]=").arg(QLocale::system().name());

//        if (lineContent.startsWith(localNameFlag)) {
//            displayName = lineContent.remove(0, localNameFlag.size());

//            break;
//        } else if (lineContent.startsWith(genericNameFlag)) {
//            displayName = lineContent.remove(0, genericNameFlag.size());
//            break;
//        } else if (lineContent.startsWith(nameFlag)) {
//            displayName = lineContent.remove(0, nameFlag.size());
//            continue;
//        } else {
//            continue;
//        }
//    }
//    in.close();

//    return displayName;
//}


StartupListWidget::StartupListWidget(QWidget *parent) : QListWidget(parent)
{
//    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setFixedWidth(parent->width());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, &StartupListWidget::itemDoubleClicked, [=] (QListWidgetItem *item) {
            StartupItem *fileItem = static_cast<StartupItem *>(itemWidget(item));
            qDebug() << fileItem->getAppName();
        });

//    loadItems(QStringList(), 0);
    QStringList tmp;
    tmp.append("AAA");
    tmp.append("BBB");
    tmp.append("CCC");
    tmp.append("DDD");
    tmp.append("EEE");
    tmp.append("FFF");
    tmp.append("GGG");
    tmp.append("HHH");
    tmp.append("JJJ");
    tmp.append("KKK");
    tmp.append("III");
    loadItems(tmp, 0);
}

StartupListWidget::~StartupListWidget()
{
    for(int i = 0; i < this->count(); i++) {
        QListWidgetItem *listItem = item(i);
        StartupItem *item = static_cast<StartupItem *>(itemWidget(listItem));
        if (item) {
            item->deleteLater();
        }
    }
}

void StartupListWidget::loadItem(QString info)
{
    StartupItem *item = new StartupItem();
    item->setAppName(info);
    connect(item, SIGNAL(changeStartup()), this, SLOT(onChangeStartup()));
    connect(item, SIGNAL(enter()), this, SLOT(onMouseEnter()));
    addItem(item->getItem());
    item->getItem()->setSizeHint(QSize(this->width() - 10, 60));
    setItemWidget(item->getItem(), item);
}

void StartupListWidget::loadItems(QStringList items, int scrollValue)
{
    clear();

    foreach (auto item, items) {
        loadItem(item);
    }

    this->verticalScrollBar()->setValue(scrollValue);
}

void StartupListWidget::onChangeStartup()
{
    QString appName = ((StartupItem*) sender())->getAppName();









//    def interface_get_status(fobj):
//        locale_language = locale.getdefaultlocale()[0]
//        try:
//            obj = Desktop_Autostart_Manage()
//            obj.get_final_status()
//            up = obj.dic.get("autostart", [])
//            if up:
//                for upvalue in up:
//                    up_list = obj.get_desktop_info(upvalue, locale_language)
//                    up_list.append('Status:' + 'true')
//                    fobj.autostartmanage_data_signal(up_list)

//            down = obj.dic.get("notautostart", [])
//            if down:
//                for downvalue in down:
//                    down_list = obj.get_desktop_info(downvalue, locale_language)
//                    down_list.append('Status:' + 'false')
//                    fobj.autostartmanage_data_signal(down_list)


//    std::string desktopFile;
//    if (trayProcessMap.contains(pid)) {
//        desktopFile = Utils::getProcessEnvironmentVariable(pid, "GIO_LAUNCHED_DESKTOP_FILE").toStdString();
//    } else {
//        desktopFile = getDesktopFileFromName(pid, name, cmdline);
//    }

//    QPixmap icon;
//    if (desktopFile.size() == 0) {
//        qreal devicePixelRatio = qApp->devicePixelRatio();
//        icon = findWindowTitle->getWindowIcon(findWindowTitle->getWindow(pid), 96 * devicePixelRatio);
//        icon.setDevicePixelRatio(devicePixelRatio);
//    } else {
//        icon = getDesktopFileIcon(desktopFile, 96);
//    }
//    QString displayName = getDisplayNameFromName(name, desktopFile, false);

}

void StartupListWidget::onMouseEnter()
{
    for(int i = 0; i < this->count(); i++) {
        QListWidgetItem *listItem = item(i);
        StartupItem *item = static_cast<StartupItem *>(itemWidget(listItem));
        if (item->getAppName() == ((StartupItem*) sender())->getAppName()) {
            item->setItemHovered();
        } else {
            item->unsetItemHovered();
        }
    }
}
