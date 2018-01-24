#ifndef STARTUPITEM_H
#define STARTUPITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

class KylinSwitcher;

class StartupItem : public QWidget
{
    Q_OBJECT

public:
    StartupItem(QWidget *parent=0);

    QListWidgetItem* getItem();
    QString getAppName();
    void setAppName(const QString &name);

    void setItemHovered();
    void unsetItemHovered();

signals:
    void changeStartup();
    void enter();

protected:
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *);

private:
    bool isEntered;
    QListWidgetItem *item = nullptr;
    QString m_appName;

    QLabel *m_appIcon = nullptr;
    QLabel *m_appNameLabel = nullptr;
    QLabel *m_appDescLabel = nullptr;
    KylinSwitcher *switcher = nullptr;

    QWidget *m_labelWidget = nullptr;

    QHBoxLayout *m_switchLayout = nullptr;
    QVBoxLayout *m_labelLayout = nullptr;
    QHBoxLayout *m_leftLayout = nullptr;
    QHBoxLayout *m_layout = nullptr;
};

#endif // STARTUPITEM_H
