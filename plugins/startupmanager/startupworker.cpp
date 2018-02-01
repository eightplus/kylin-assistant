/*
 * Copyright (C) 2013 ~ 2018 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "startupworker.h"

#include <QSettings>
#include <QDebug>
#include <QFileInfo>

#include <glib.h>
#include <sys/stat.h>
#include <string.h>
#include "util.h"


#define gsp_key_file_get_string(key_file, key) \
         g_key_file_get_string (key_file, G_KEY_FILE_DESKTOP_GROUP, key, NULL)
#define gsp_key_file_get_locale_string(key_file, key) \
         g_key_file_get_locale_string(key_file, G_KEY_FILE_DESKTOP_GROUP, key, NULL, NULL)

#define gsp_key_file_set_boolean(key_file, key, value) \
         g_key_file_set_boolean (key_file, G_KEY_FILE_DESKTOP_GROUP, key, value)
#define gsp_key_file_set_string(key_file, key, value) \
         g_key_file_set_string (key_file, G_KEY_FILE_DESKTOP_GROUP, key, value)

gboolean gsp_key_file_get_boolean (GKeyFile    *keyfile,
                          const gchar *key,
                          gboolean     default_value)
{
        GError   *error;
        gboolean  retval;

        error = NULL;
        retval = g_key_file_get_boolean (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                         key, &error);
        if (error != NULL) {
                retval = default_value;
                g_error_free (error);
        }

        return retval;
}

gboolean gsp_key_file_to_file (GKeyFile     *keyfile,
                      const gchar  *path,
                      GError      **error)
{
        GError  *write_error;
        gchar   *data;
        gsize    length;
        gboolean res;

        g_return_val_if_fail (keyfile != NULL, FALSE);
        g_return_val_if_fail (path != NULL, FALSE);

        write_error = NULL;
        data = g_key_file_to_data (keyfile, &length, &write_error);

        if (write_error) {
                g_propagate_error (error, write_error);
                return FALSE;
        }

        res = g_file_set_contents (path, data, length, &write_error);
        g_free (data);

        if (write_error) {
                g_propagate_error (error, write_error);
                return FALSE;
        }

        return res;
}

void gsp_key_file_populate (GKeyFile *keyfile)
{
        gsp_key_file_set_string (keyfile,
                                 G_KEY_FILE_DESKTOP_KEY_TYPE,
                                 "Application");

        gsp_key_file_set_string (keyfile,
                                 G_KEY_FILE_DESKTOP_KEY_EXEC,
                                 "/bin/false");
}

void gsp_key_file_set_locale_string (GKeyFile    *keyfile,
                                const gchar *key,
                                const gchar *value)
{
        const char         *locale;
        const char * const *langs_pointer;
        int                 i;

        if (value == NULL) {
                value = "";
        }

        locale = NULL;
        langs_pointer = g_get_language_names ();
        for (i = 0; langs_pointer[i] != NULL; i++) {
                /* find first without encoding  */
                if (strchr (langs_pointer[i], '.') == NULL) {
                        locale = langs_pointer[i];
                        break;
                }
        }

        if (locale != NULL) {
                g_key_file_set_locale_string (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                              key, locale, value);
        } else {
                g_key_file_set_string (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                       key, value);
        }
}

gboolean gsp_key_file_get_shown (GKeyFile   *keyfile,
                        const char *current_desktop)
{
        char     **only_show_in, **not_show_in;
        gboolean   found;
        int        i;

        if (!current_desktop)
                return TRUE;

        only_show_in = g_key_file_get_string_list (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                                   G_KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN,
                                                   NULL, NULL);

        if (only_show_in) {
                found = FALSE;
                for (i = 0; only_show_in[i] != NULL; i++) {
                        if (g_strcmp0 (current_desktop, only_show_in[i]) == 0) {
                                found = TRUE;
                                break;
                        }
                }

                g_strfreev (only_show_in);

                if (!found)
                        return FALSE;
        }

        not_show_in = g_key_file_get_string_list (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                                  G_KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN,
                                                  NULL, NULL);
        if (not_show_in) {
                found = FALSE;
                for (i = 0; not_show_in[i] != NULL; i++) {
                        if (g_strcmp0 (current_desktop, not_show_in[i]) == 0) {
                                found = TRUE;
                                break;
                        }
                }

                g_strfreev (not_show_in);

                if (found)
                        return FALSE;
        }

        return TRUE;
}

static char *_gsp_get_current_desktop ()
{
        static char *current_desktop = NULL;

        /* Support XDG_CURRENT_DESKTOP environment variable; this can be used
         * to abuse gnome-session in non-GNOME desktops. */
        if (!current_desktop) {
               const char *desktop;

                desktop = g_getenv ("XDG_CURRENT_DESKTOP");

                /* Note: if XDG_CURRENT_DESKTOP is set but empty, do as if it
                 * was not set */
                if (!desktop || desktop[0] == '\0')
                        current_desktop = g_strdup ("GNOME");
                else
                        current_desktop = g_strdup (desktop);
        }

        /* Using "*" means skipping desktop-related checks */
        if (g_strcmp0 (current_desktop, "*") == 0)
                return NULL;

        return current_desktop;
}

static gboolean _gsp_str_equal (const char *a,
                const char *b)
{
        if (g_strcmp0 (a, b) == 0) {
                return TRUE;
        }

        if (a && !b && a[0] == '\0') {
                return TRUE;
        }

        if (b && !a && b[0] == '\0') {
                return TRUE;
        }

        return FALSE;
}




//#include <gio/gio.h>
//static gboolean
//gsp_app_manager_xdg_dir_monitor (GFileMonitor      *monitor,
//                                 GFile             *child,
//                                 GFile             *other_file,
//                                 GFileMonitorEvent  flags,
//                                 gpointer           data)
//{
////        GspAppManager *manager;
////        GspApp        *old_app;
////        GspApp        *app;
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

//GFileMonitor *monitor;
//monitor = g_file_monitor_directory (file, G_FILE_MONITOR_NONE, NULL, NULL);
//if (monitor) {
//    g_signal_connect (monitor, "changed", G_CALLBACK (gsp_app_manager_xdg_dir_monitor), manager);
//}
//g_file_monitor_cancel (monitor);
//g_object_unref (monitor);
//monitor = NULL;


void gsp_key_file_ensure_C_key (GKeyFile   *keyfile, const char *key)
{
        char *C_value;
        char *buffer;

        /* Make sure we set the "C" locale strings to the terms we set here.
         * This is so that if the user logs into another locale they get their
         * own description there rather then empty. It is not the C locale
         * however, but the user created this entry herself so it's OK */
        C_value = gsp_key_file_get_string (keyfile, key);
        if (C_value == NULL || C_value [0] == '\0') {
                buffer = gsp_key_file_get_locale_string (keyfile, key);
                if (buffer) {
                        gsp_key_file_set_string (keyfile, key, buffer);
                        g_free (buffer);
                }
        }
        g_free (C_value);
}

inline QString getCurrentDesktopEnvironment()
{
    QString current_desktop;

    current_desktop = qgetenv("XDG_CURRENT_DESKTOP");//g_getenv
    if(current_desktop.isEmpty()) {
        current_desktop = qgetenv("XDG_SESSION_DESKTOP");
        if(current_desktop.isEmpty())
            current_desktop = "GNOME";
    }

    return current_desktop;
}

bool getShownFromDesktopFile(const QString &desktopFile, const QString &desktopEnvironment)
{
    if (desktopEnvironment.isNull() || desktopEnvironment.isEmpty())
        return true;

    bool found;
    QSettings setting(desktopFile, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);
    QStringList onlyShowIn = setting.value(KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN).toStringList();
    QStringList notShowIn = setting.value(KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN).toStringList();
    setting.endGroup();

    if (!onlyShowIn.isEmpty()) {
        found = false;
        foreach (auto dekstopEnv, onlyShowIn) {
            if (dekstopEnv == desktopEnvironment) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    if (!notShowIn.isEmpty()) {
        found = false;
        foreach (auto dekstopEnv, notShowIn) {
            if (dekstopEnv == desktopEnvironment) {
                found = true;
                break;
            }
        }
        if (found)
            return false;
    }

    return true;
}

StartupWorker::StartupWorker(QObject *parent)
    : QObject(parent)
{

}

StartupWorker::~StartupWorker()
{
    m_startupInfoList.clear();
    this->dirs.clear();
}

void StartupWorker::AddDirWithIndex(GspXdgDir dir)
{
    this->dirs.append(dir);
}

int StartupWorker::getDirIndex(QString dir)
{
    foreach (GspXdgDir item, this->dirs) {
        if (item.dir == dir) {
            return item.index;
        }
    }
    return -1;
}

QString StartupWorker::gsp_app_manager_get_dir(unsigned int index)
{
    foreach (GspXdgDir item, this->dirs) {
        if (item.index == index) {
            return item.dir;
        }
    }

    return QString();
}


StartupData StartupWorker::gsp_app_manager_find_app_with_basename(QString &basename)
{
    for (StartupData info : this->getStartupInfoList()) {
        if (info.basename == basename) {
            return info;
        }
    }

    return StartupData();
}

void StartupWorker::newStartupInfo(const QString &desktopFile, unsigned int xdg_position)
{
    bool isNew;
    QString basename = QFileInfo(desktopFile).fileName();

    StartupData info = gsp_app_manager_find_app_with_basename(basename);
    if (info.basename.isEmpty() && info.name.isEmpty() && info.exec.isEmpty())
        isNew = true;
    else
        isNew = false;

    if (!isNew) {
        if (info.xdg_position == xdg_position) {
//            if (app->priv->skip_next_monitor_event) {
//                app->priv->skip_next_monitor_event = FALSE;
//                return;
//            }
            /* else: the file got changed but not by us, we'll
             * update our data from disk */
        }
        if (info.xdg_position < xdg_position) {
            /* we don't really care about this file, since we
             * already have something with a higher priority, or
             * we're going to write something in the user config
             * anyway.
             * Note: xdg_position >= 1 so it's a system dir */
            info.xdg_system_position = MIN (xdg_position, info.xdg_system_position);
            return;
        }
    }

    GKeyFile      *keyfile;
    keyfile = g_key_file_new ();
    if (!g_key_file_load_from_file (keyfile, desktopFile.toStdString().c_str(), G_KEY_FILE_NONE, NULL)) {
        g_key_file_free (keyfile);
        return;
    }

    bool hidden = gsp_key_file_get_boolean (keyfile, G_KEY_FILE_DESKTOP_KEY_HIDDEN, FALSE);
    bool no_display = gsp_key_file_get_boolean (keyfile,G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY,FALSE);
    bool enabled = gsp_key_file_get_boolean (keyfile, KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED,TRUE);
    bool shown = gsp_key_file_get_shown (keyfile,_gsp_get_current_desktop ());

    std::string formatted_result(make_string(gsp_key_file_get_locale_string (keyfile, G_KEY_FILE_DESKTOP_KEY_NAME)));
    QString name = QString::fromStdString(formatted_result);
    formatted_result = make_string(gsp_key_file_get_string (keyfile, G_KEY_FILE_DESKTOP_KEY_EXEC));
    QString exec = QString::fromStdString(formatted_result);
    formatted_result = make_string(gsp_key_file_get_locale_string (keyfile, G_KEY_FILE_DESKTOP_KEY_COMMENT));
    QString comment = QString::fromStdString(formatted_result);
    formatted_result = make_string(gsp_key_file_get_locale_string (keyfile, G_KEY_FILE_DESKTOP_KEY_ICON));
    QString icon = QString::fromStdString(formatted_result);
    if (name.isEmpty() || name.isNull())
        name = exec;

    /*QSettings setting(desktopFile, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);

    bool hidden = setting.value(KEY_FILE_DESKTOP_KEY_HIDDEN, false).toBool();
    bool no_display = setting.value(KEY_FILE_DESKTOP_KEY_NO_DISPLAY, false).toBool();
    bool enabled = setting.value(KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED, true).toBool();
    bool shown = getShownFromDesktopFile(desktopFile, getCurrentDesktopEnvironment());

    QString name = setting.value(KEY_FILE_DESKTOP_KEY_NAME).toString();
    QString exec = setting.value(KEY_FILE_DESKTOP_KEY_EXEC).toString();
    QString comment = setting.value(KEY_FILE_DESKTOP_KEY_COMMENT).toString();
    QString icon = setting.value(KEY_FILE_DESKTOP_KEY_ICON).toString();
    if (name.isEmpty() || name.isNull())
        name = exec;
    setting.endGroup();*/

    //get description
    QString primary;
    QString secondary;
    if (!name.isEmpty()) {
        primary = name;
    } else if (!exec.isEmpty()) {
        primary = exec;
    } else {
        primary = tr("No name");
    }
    if (!comment.isEmpty()) {
        secondary = comment;
    } else {
        secondary = tr("No description");
    }
    QString description = QString("<b>%1</b>\n%2").arg(primary).arg(secondary);

    info.basename = basename;
    info.path = desktopFile;
    info.hidden = hidden;
    info.no_display = no_display;
    info.enabled = enabled;
    info.shown = shown;
    info.name = name;
    info.exec = exec;
    info.comment = comment;
    info.icon = icon;
    info.description = description;

    if (xdg_position > 0) {
        info.xdg_system_position = xdg_position;
    }
    /* else we keep the old value (which is G_MAXUINT if it wasn't set) */
    info.xdg_position = xdg_position;
    info.old_system_path.clear();

    //printf("hidden=%s\n", hidden ? "Yes" : "No");

    m_startupInfoList[exec] = info;//append item

/*
    //show or hide
    if (!hidden && shown && !no_display) {
        //show
//        if (isExecContains(exec)) {

//        }
        m_startupInfoList[exec] = info;

    }
    else {
        //hide
        if (isExecContains(exec))
            m_startupInfoList.remove(exec);
    }
    */
}

bool StartupWorker::isExecContains(const QString &exec)
{
    return m_startupInfoList.keys().contains(exec);
}

QList<StartupData> StartupWorker::getStartupInfoList() const
{
    return m_startupInfoList.values();
}

StartupData StartupWorker::getStartupInfo(const QString &exec)
{
    return m_startupInfoList.value(exec, StartupData());//nullptr
}

void StartupWorker::updateEnable(const QString &exec, bool enabled)
{
//    m_startupInfoList.value(exec).enabled = enabled;
//    m_startupInfoList.value(exec).setEnabled(enabled);
    if (m_startupInfoList.contains(exec)) {
        m_startupInfoList[exec].enabled = enabled;//m_startupInfoList[exec].setEnabled(enabled);
    }
}

void StartupWorker::updateSaveMask(const QString &exec, unsigned int save_mask)
{
    if (m_startupInfoList.contains(exec)) {
        m_startupInfoList[exec].save_mask = save_mask;
    }
}

void StartupWorker::updateXdgPosition(const QString &exec, unsigned int xdg_position)
{
    if (m_startupInfoList.contains(exec)) {
        m_startupInfoList[exec].xdg_position = xdg_position;
    }
}

void StartupWorker::updateXdgSystemPosition(const QString &exec, unsigned int xdg_system_position)
{
    if (m_startupInfoList.contains(exec)) {
        m_startupInfoList[exec].xdg_system_position = xdg_system_position;
    }
}

void StartupWorker::updateOldSystemPath(const QString &exec, QString old_system_path)
{
    if (m_startupInfoList.contains(exec)) {
        m_startupInfoList[exec].old_system_path = old_system_path;
    }
}

void StartupWorker::updatePath(const QString &exec, QString path)
{
    if (m_startupInfoList.contains(exec)) {
        m_startupInfoList[exec].path = path;
    }
}

void StartupWorker::_gsp_ensure_user_autostart_dir (void)
{
    char *dir;
    dir = g_build_filename(g_get_user_config_dir(), "autostart", NULL);
    g_mkdir_with_parents(dir, S_IRWXU);
    printf("ensure dir=%s\n", dir);
    g_free(dir);

//    QDir dir;
//    if(!dir.exists("aa")){
//        dir.mkdir("aa");
//    }


    /*const gchar *config_dir = g_get_user_config_dir();
    std::string formatted_result(make_string(g_strdup(config_dir)));//std::string formatted_result = make_string(g_strdup(config_dir));
    QString dir = QString::fromStdString(formatted_result);
    //S_IRWXU 00700 权限，代表该文件所有者具有可读、可写及可执行的权限
    if (dir.endsWith("/"))
        g_mkdir_with_parents (dir + "autostart", S_IRWXU);//if (g_mkdir_with_parents (dir, 0755) == 0)
    else
        g_mkdir_with_parents (dir + "/autostart", S_IRWXU);*/
}

void StartupWorker::_gsp_app_save_done_success (StartupData info)
{
    info.save_mask = 0;
    this->updateSaveMask(info.exec, info.save_mask);
    if (!info.old_system_path.isEmpty()) {
        info.old_system_path.clear();
        this->updateOldSystemPath(info.exec, info.old_system_path);
    }
}

bool StartupWorker::_gsp_app_user_equal_system (StartupData info, char **system_path)
{
        QString system_dir;
        char          *path;
        char          *str;
        GKeyFile      *keyfile;

        system_dir = gsp_app_manager_get_dir(info.xdg_system_position);
        if (system_dir.isEmpty()) {
            return false;
        }

        path = g_build_filename(system_dir.toStdString().c_str(), info.basename.toStdString().c_str(), NULL);

        keyfile = g_key_file_new();
        if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, NULL)) {
            g_free (path);
            g_key_file_free (keyfile);
            return false;
        }

        if (gsp_key_file_get_boolean (keyfile,
                                      G_KEY_FILE_DESKTOP_KEY_HIDDEN,
                                      FALSE) != info.hidden ||
            gsp_key_file_get_boolean (keyfile,
                                      KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED,
                                      TRUE) != info.enabled ||
            gsp_key_file_get_shown (keyfile,
                                    _gsp_get_current_desktop ()) != info.shown) {
                g_free (path);
                g_key_file_free (keyfile);
                return false;
        }
        if (gsp_key_file_get_boolean (keyfile,
                                      G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY,
                                      FALSE) != info.no_display) {
                g_free (path);
                g_key_file_free (keyfile);
                return false;
        }

        str = gsp_key_file_get_locale_string (keyfile, G_KEY_FILE_DESKTOP_KEY_NAME);
        if (!_gsp_str_equal (str, info.name.toStdString().c_str())) {
                g_free (str);
                g_free (path);
                g_key_file_free (keyfile);
                return false;
        }
        g_free (str);

        str = gsp_key_file_get_locale_string (keyfile, G_KEY_FILE_DESKTOP_KEY_COMMENT);
        if (!_gsp_str_equal (str, info.comment.toStdString().c_str())) {
                g_free (str);
                g_free (path);
                g_key_file_free (keyfile);
                return false;
        }
        g_free (str);

        str = gsp_key_file_get_string (keyfile,
                                       G_KEY_FILE_DESKTOP_KEY_EXEC);
        if (!_gsp_str_equal (str, info.exec.toStdString().c_str())) {
                g_free (str);
                g_free (path);
                g_key_file_free (keyfile);
                return false;
        }
        g_free (str);
        str = gsp_key_file_get_locale_string (keyfile,
                                              G_KEY_FILE_DESKTOP_KEY_ICON);
        if (!_gsp_str_equal (str, info.icon.toStdString().c_str())) {//info.icon.toStdString().data()
                g_free (str);
                g_free (path);
                g_key_file_free (keyfile);
                return false;
        }
        g_free (str);

        g_key_file_free (keyfile);

        *system_path = path;

        return true;
}

/*bool StartupWorker::_gsp_app_user_equal_system (StartupData info, QString &system_path, QString locale)
{
    QString system_dir;
    QString path;
    QString str;

    qDebug() << "info.xdg_system_position="<<info.xdg_system_position;
    system_dir = gsp_app_manager_get_dir(info.xdg_system_position);
    if (system_dir.isEmpty()) {
        return false;
    }

    path = QString("%1/%2").arg(system_dir).arg(info.basename);
    qDebug() << "_gsp_app_user_equal_system path="<<path;

    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);

    if (setting.value(KEY_FILE_DESKTOP_KEY_HIDDEN, false).toBool() != info.hidden
            || setting.value(KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED, true).toBool() != info.enabled
            || getShownFromDesktopFile(path, getCurrentDesktopEnvironment()) != info.shown) {
        setting.endGroup();
        return false;
    }

    if (setting.value(KEY_FILE_DESKTOP_KEY_NO_DISPLAY, false).toBool() != info.no_display) {
        setting.endGroup();
        return false;
    }

    str = setting.value(QString("%1\[%2\]").arg(KEY_FILE_DESKTOP_KEY_NAME).arg(locale)).toString();
    if (QString::compare(str, info.name) != 0) {
        setting.endGroup();
        return false;
    }
    str = setting.value(QString("%1\[%2\]").arg(KEY_FILE_DESKTOP_KEY_COMMENT).arg(locale)).toString();
    if (QString::compare(str, info.comment) != 0) {
        setting.endGroup();
        return false;
    }

    str = setting.value(KEY_FILE_DESKTOP_KEY_EXEC).toString();
    if (QString::compare(str, info.exec) != 0) {
        setting.endGroup();
        return false;
    }

    str = setting.value(QString("%1\[%2\]").arg(KEY_FILE_DESKTOP_KEY_ICON).arg(locale)).toString();
    if (QString::compare(str, info.icon) != 0) {
        setting.endGroup();
        return false;
    }
    system_path = path;

    setting.endGroup();

    return true;
}*/

//void StartupWorker::gsp_key_file_ensure_C_key(QString filename, QString key, QString locale)
//{
//    QSettings setting(filename, QSettings::IniFormat);
//    setting.setIniCodec("UTF-8");
//    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);
//    QString C_value;
//    /* Make sure we set the "C" locale strings to the terms we set here.
//     * This is so that if the user logs into another locale they get their
//     * own description there rather then empty. It is not the C locale
//     * however, but the user created this entry herself so it's OK */
//    C_value = setting.value(key).toString();
//    if (C_value.isEmpty() || C_value.isNull()) {
//        QString buffer = setting.value(QString("%1\[%2\]").arg(key).arg(locale)).toString();//g_key_file_get_locale_string
//        if (!buffer.isEmpty()) {
//            setting.setValue(key, buffer);
//        }
//    }
//    setting.endGroup();
//    setting.sync();
//}



bool StartupWorker::_gsp_app_save(StartupData info)
{
    char     *use_path;
    GKeyFile *keyfile;
    GError   *error;

    /* first check if removing the data from the user dir and using the
     * data from the system dir is enough -- this helps us keep clean the
     * user config dir by removing unneeded files */
    if (_gsp_app_user_equal_system (info, &use_path)) {
//            if (g_file_test(info.path.toStdString().c_str(), G_FILE_TEST_EXISTS)) {
//                g_remove(info.path.toStdString().c_str());
//            }
            QFile file(info.path);
            if (file.exists()) {
                file.remove();
            }
            std::string formatted_result(make_string(g_strdup(use_path)));
            info.path = QString::fromStdString(formatted_result);
            info.xdg_position = info.xdg_system_position;
            _gsp_app_save_done_success(info);
            return false;
    }
    if (!info.old_system_path.isEmpty())
        use_path = g_strdup(info.old_system_path.toStdString().c_str());
    else
        use_path = g_strdup(info.path.toStdString().c_str());

    printf("normal use_path=%s", use_path);

    keyfile = g_key_file_new ();

    error = NULL;
    GKeyFileFlags flags;
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    g_key_file_load_from_file (keyfile, use_path, flags, &error);
    g_free(use_path);

    if (error) {
            g_error_free (error);
            gsp_key_file_populate (keyfile);
    }

    if (info.save_mask & GSP_ASP_SAVE_MASK_HIDDEN) {
            gsp_key_file_set_boolean (keyfile,
                                      G_KEY_FILE_DESKTOP_KEY_HIDDEN,
                                      info.hidden);
    }

    if (info.save_mask & GSP_ASP_SAVE_MASK_NO_DISPLAY) {
            gsp_key_file_set_boolean (keyfile,
                                      G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY,
                                      info.no_display);
    }

    if (info.save_mask & SAVE_MASK_ENABLED) {
            gsp_key_file_set_boolean (keyfile,
                                      KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED,
                                      info.enabled);
    }

    if (info.save_mask & GSP_ASP_SAVE_MASK_NAME) {
            gsp_key_file_set_locale_string (keyfile,
                                            G_KEY_FILE_DESKTOP_KEY_NAME,
                                            info.name.toStdString().c_str());
            gsp_key_file_ensure_C_key (keyfile, G_KEY_FILE_DESKTOP_KEY_NAME);
    }
    if (info.save_mask & GSP_ASP_SAVE_MASK_COMMENT) {
            gsp_key_file_set_locale_string (keyfile,
                                            G_KEY_FILE_DESKTOP_KEY_COMMENT,
                                            info.comment.toStdString().c_str());
            gsp_key_file_ensure_C_key (keyfile, G_KEY_FILE_DESKTOP_KEY_COMMENT);
    }

    if (info.save_mask & GSP_ASP_SAVE_MASK_EXEC) {
            gsp_key_file_set_string (keyfile,
                                     G_KEY_FILE_DESKTOP_KEY_EXEC,
                                     info.exec.toStdString().c_str());
    }

    _gsp_ensure_user_autostart_dir ();
    if (gsp_key_file_to_file (keyfile, info.path.toStdString().c_str(), NULL)) {
        _gsp_app_save_done_success(info);
    } else {
        qDebug() << QString("Could not save %1 file").arg(info.path);
    }

    g_key_file_free (keyfile);

    return false;






/*
    QString use_path;

    QString locale = QLocale::system().name();
//    qDebug() << "locale="<<locale;

    //first check if removing the data from the user dir and using the
    //data from the system dir is enough -- this helps us keep clean the
    //user config dir by removing unneeded files
    if (_gsp_app_user_equal_system(info, use_path, locale)) {
        qDebug() << "system use_path="<<use_path;
        QFile file(info.path);
        if (file.exists()) {
            file.remove();
        }
        info.path = use_path;
        this->updatePath(info.exec, info.path);
        info.xdg_position = info.xdg_system_position;
        this->updateXdgPosition(info.exec, info.xdg_position);
        _gsp_app_save_done_success(info);
        return false;
    }

    if (!info.old_system_path.isEmpty())
        use_path = info.old_system_path;
    else
        use_path = info.path;

    qDebug() << "normal use_path="<<use_path;

//    const char         *locale;
//    const char * const *langs_pointer;
//    int                 i;
//    locale = NULL;
//    langs_pointer = g_get_language_names ();
//    for (i = 0; langs_pointer[i] != NULL; i++) {
//        printf("langs_pointer[i]=%s\n", langs_pointer[i]);
//            //find first without encoding
//            if (strchr (langs_pointer[i], '.') == NULL) {
//                    locale = langs_pointer[i];
//                    break;
//            }
//    }
//    if (locale != NULL) {
//        printf("locale=%s\n", locale);//locale=zh_CN
//    }

    //if read error:set KEY_FILE_DESKTOP_KEY_TYPE "Application" and set KEY_FILE_DESKTOP_KEY_EXEC "/bin/false"
    QSettings setting(use_path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);
    if (info.save_mask & GSP_ASP_SAVE_MASK_HIDDEN) {
        setting.setValue(KEY_FILE_DESKTOP_KEY_HIDDEN, info.hidden);
    }
    if (info.save_mask & GSP_ASP_SAVE_MASK_NO_DISPLAY) {
        setting.setValue(KEY_FILE_DESKTOP_KEY_NO_DISPLAY, info.no_display);
    }
    if (info.save_mask & SAVE_MASK_ENABLED) {
        setting.setValue(KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED, info.enabled);
    }
    if (info.save_mask & GSP_ASP_SAVE_MASK_NAME) {
        setting.setValue(QString("%1\[%2\]").arg(KEY_FILE_DESKTOP_KEY_NAME).arg(locale), info.name);
//        gsp_key_file_ensure_C_key(use_path, KEY_FILE_DESKTOP_KEY_NAME, locale);
    }
    if (info.save_mask & GSP_ASP_SAVE_MASK_COMMENT) {
        setting.setValue(QString("%1\[%2\]").arg(KEY_FILE_DESKTOP_KEY_COMMENT).arg(locale), info.comment);
//        gsp_key_file_ensure_C_key(use_path, KEY_FILE_DESKTOP_KEY_NAME, locale);
    }
    if (info.save_mask & GSP_ASP_SAVE_MASK_EXEC) {
        setting.setValue(KEY_FILE_DESKTOP_KEY_EXEC, info.exec);
    }
    _gsp_ensure_user_autostart_dir();

    setting.endGroup();
    setting.sync();
    _gsp_app_save_done_success(info);

    return false;*/
}


void StartupWorker::_gsp_app_queue_save(StartupData info)
{
    /* if the file was not in the user directory, then we'll create a copy
     * there */
    if (info.xdg_position != 0) {
        info.xdg_position = 0;
        this->updateXdgPosition(info.exec, info.xdg_position);
        if (info.old_system_path.isEmpty()) {
                info.old_system_path = info.path;
                this->updateOldSystemPath(info.exec, info.old_system_path);
                /* if old_system_path was not NULL, then it means we
                 * tried to save and we failed; in that case, we want
                 * to try again and use the old file as a basis again */
        }
        const gchar *config_dir = g_get_user_config_dir();
        std::string formatted_result(make_string(g_strdup(config_dir)));
        QString tmpPath = QString::fromStdString(formatted_result);
        if (tmpPath.endsWith("/"))
            info.path = QString("%1/autostart/%2").arg(tmpPath).arg(info.basename);
        else
            info.path = QString("%1/autostart/%2").arg(tmpPath).arg(info.basename);
        this->updatePath(info.exec, info.path);
    }
    _gsp_app_save(info);
}
