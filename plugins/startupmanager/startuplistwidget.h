#ifndef STARTUPLISTWIDGET_H
#define STARTUPLISTWIDGET_H

#include "startupitem.h"

#include <QListWidget>

class StartupListWidget : public QListWidget
{
    Q_OBJECT

public:
    StartupListWidget(QWidget *parent=0);
    ~StartupListWidget();

    void loadItem(QString info);
    void loadItems(QStringList items, int scrollValue);

public slots:
    void onChangeStartup();
    void onMouseEnter();
};

#endif // STARTUPLISTWIDGET_H
