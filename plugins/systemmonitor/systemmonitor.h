#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include "titlewidget.h"
#include "toolbar.h"
#include "processdialog.h"
#include "resourcesdialog.h"
#include "filesystemdialog.h"

#include <QFrame>
#include <QStackedWidget>
#include <QLabel>
#include <QSettings>

enum PDragState {NOT_PDRAGGING, START_PDRAGGING, PDRAGGING};

class SystemMonitor : public QFrame
{
    Q_OBJECT

public:
    SystemMonitor(QWidget *parent = 0);
    ~SystemMonitor();

    void initTitleWidget();
    void initToolBar();
    void initPanelStack();
    void initConnections();

    QList<bool> getColumnHideFlags();
    bool getSortOrder();
    int getSortIndex();
    void moveCenter();
    void moveDialog(QPoint diff);

public slots:
    void recordVisibleColumn(int, bool, QList<bool> columnVisible);
    void recordSortStatus(int index, bool sortOrder);
    void onChangePage(int index);

protected:
    void resizeEvent(QResizeEvent *e) override;
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);

private:
    QStackedWidget  *m_sysMonitorStack = nullptr;
    TitleWidget  *m_titleWidget = nullptr;
    ToolBar *m_toolBar = nullptr;
    ProcessDialog *process_dialog = nullptr;
    ResouresDialog *resources_dialog = nullptr;
    FileSystemDialog *filesystem_dialog = nullptr;
    PDragState drag_state;
    QPoint start_drag;
    QSettings *proSettings;
};

#endif // SYSTEMMONITOR_H
