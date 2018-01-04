#include "systemmonitor.h"

#include "util.h"
#include <QFileSystemWatcher>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QFile>
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>

namespace {
const int MAINWIDGET_MINIMUN_HEIGHT = 480;
const int MAINWIDGET_MINIMUN_WIDTH = 640;
}

SystemMonitor::SystemMonitor(QWidget *parent)
    : QFrame(parent)
    , drag_state(NOT_PDRAGGING)
    , start_drag(QPoint(0,0))
{
    /*this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#0d87ca"));
    this->setPalette(palette);*/

//    this->setStyleSheet("QFrame{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
//    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    this->setMouseTracking(true);
    installEventFilter(this);

    this->resize(900, 600);
    setMinimumSize(MAINWIDGET_MINIMUN_WIDTH, MAINWIDGET_MINIMUN_HEIGHT);

    proSettings = new QSettings(KYLIN_COMPANY_SETTING, KYLIN_SETTING_FILE_NAME_SETTING);
    proSettings->setIniCodec("UTF-8");

    this->initTitleWidget();
    this->initToolBar();
    this->initPanelStack();
    this->initConnections();

    this->moveCenter();
}

SystemMonitor::~SystemMonitor()
{
    if (m_sysMonitorStack) {
        foreach (QObject *child, m_sysMonitorStack->children()) {
            QWidget *widget = static_cast<QWidget *>(child);
            widget->deleteLater();
        }
    }
    if (m_titleWidget) {
        delete m_titleWidget;
        m_titleWidget = nullptr;
    }
    if (m_toolBar) {
        delete m_toolBar;
        m_toolBar = nullptr;
    }
    if (process_dialog) {
        delete process_dialog;
        process_dialog = nullptr;
    }
    if (resources_dialog) {
        delete resources_dialog;
        resources_dialog = nullptr;
    }
    if (filesystem_dialog) {
        delete filesystem_dialog;
        filesystem_dialog = nullptr;
    }
    if (proSettings != NULL) {
        delete proSettings;
        proSettings = NULL;
    }
}

void SystemMonitor::resizeEvent(QResizeEvent *e)
{
    if (m_titleWidget) {
        m_titleWidget->resize(width(), TOP_TITLE_WIDGET_HEIGHT);
        if (e->oldSize()  != e->size()) {
            emit m_titleWidget->updateMaxBtn();
        }
    }
    if (m_toolBar) {
        m_toolBar->resize(width(), TOP_TITLE_WIDGET_HEIGHT);
        m_toolBar->move(0, TOP_TITLE_WIDGET_HEIGHT);
    }
    if (m_sysMonitorStack) {
        m_sysMonitorStack->resize(width(), this->height() - TOP_TITLE_WIDGET_HEIGHT*2);
        m_sysMonitorStack->move(0, TOP_TITLE_WIDGET_HEIGHT*2);
    }
}

void SystemMonitor::recordVisibleColumn(int, bool, QList<bool> columnVisible)
{
    QList<QString> m_visibleColumns;
    m_visibleColumns << "name";

    if (columnVisible[1]) {
        m_visibleColumns << "user";
    }

    if (columnVisible[2]) {
        m_visibleColumns << "status";
    }

    if (columnVisible[3]) {
        m_visibleColumns << "cpu";
    }

    if (columnVisible[4]) {
        m_visibleColumns << "pid";
    }

    if (columnVisible[5]) {
        m_visibleColumns << "command";
    }

    if (columnVisible[6]) {
        m_visibleColumns << "memory";
    }

    if (columnVisible[7]) {
        m_visibleColumns << "priority";
    }

    QString processColumns = "";
    for (int i = 0; i < m_visibleColumns.length(); i++) {
        if (i != m_visibleColumns.length() - 1) {
            processColumns += QString("%1,").arg(m_visibleColumns[i]);
        } else {
            processColumns += m_visibleColumns[i];
        }
    }

    proSettings->beginGroup("PROCESS");
    proSettings->setValue("ProcessDisplayColumns", processColumns);
    proSettings->endGroup();
    proSettings->sync();
}

void SystemMonitor::recordSortStatus(int index, bool sortOrder)
{
    QList<QString> columnNames = { "name", "user", "status", "cpu", "pid", "command", "memory", "priority"};

    proSettings->beginGroup("PROCESS");
    proSettings->setValue("ProcessCurrentSortColumn", columnNames[index]);
    proSettings->setValue("ProcessSortOrder", sortOrder);
    proSettings->endGroup();
    proSettings->sync();
}

void SystemMonitor::initPanelStack()
{
    m_sysMonitorStack = new QStackedWidget(this);
    m_sysMonitorStack->setStyleSheet("QStackedWidget{background: rgb(255, 255, 255);}");
    m_sysMonitorStack->setObjectName("SystemMonitorStack");
    m_sysMonitorStack->resize(width(), this->height() - TOP_TITLE_WIDGET_HEIGHT);
    m_sysMonitorStack->move(0, TOP_TITLE_WIDGET_HEIGHT);

    m_sysMonitorStack->setMouseTracking(false);
    m_sysMonitorStack->installEventFilter(this);

    process_dialog = new ProcessDialog(getColumnHideFlags(), getSortIndex(), getSortOrder(), proSettings);
    process_dialog->getProcessView()->installEventFilter(this);
    connect(process_dialog, &ProcessDialog::changeColumnVisible, this, &SystemMonitor::recordVisibleColumn);
    connect(process_dialog, &ProcessDialog::changeSortStatus, this, &SystemMonitor::recordSortStatus);
    connect(m_toolBar, SIGNAL(activeWhoseProcessList(int)), process_dialog, SLOT(onActiveWhoseProcess(int)));

    resources_dialog = new ResouresDialog;
    filesystem_dialog = new FileSystemDialog;

    m_sysMonitorStack->addWidget(process_dialog);
    m_sysMonitorStack->addWidget(resources_dialog);
    m_sysMonitorStack->addWidget(filesystem_dialog);
    m_sysMonitorStack->setCurrentWidget(process_dialog);
}

void SystemMonitor::initTitleWidget()
{
    m_titleWidget = new TitleWidget(this);
    m_titleWidget->resize(width(), TOP_TITLE_WIDGET_HEIGHT);
    m_titleWidget->move(0, 0);
}

void SystemMonitor::initToolBar()
{
    m_toolBar = new ToolBar(proSettings, this);
    m_toolBar->resize(width(), TOP_TITLE_WIDGET_HEIGHT);
    m_toolBar->move(0, TOP_TITLE_WIDGET_HEIGHT);
}

void SystemMonitor::initConnections()
{
    connect(m_toolBar, SIGNAL(changePage(int)), this, SLOT(onChangePage(int)));
    connect(m_toolBar, SIGNAL(canelSearchEditFocus()), process_dialog, SLOT(focusProcessView()));
    connect(m_toolBar, SIGNAL(searchSignal(QString)), process_dialog, SLOT(onSearch(QString)), Qt::QueuedConnection);
}

void SystemMonitor::onChangePage(int index)
{
    if (m_sysMonitorStack) {
        m_sysMonitorStack->setCurrentIndex(index);
    }
}

int SystemMonitor::getSortIndex()
{
    proSettings->beginGroup("PROCESS");
    QString sortingName = proSettings->value("ProcessCurrentSortColumn").toString();
    proSettings->endGroup();

    QList<QString> columnNames = {
        "name", "user", "status", "cpu", "pid", "command", "memory", "priority"
    };

    return columnNames.indexOf(sortingName);
}

bool SystemMonitor::getSortOrder()
{
    proSettings->beginGroup("PROCESS");
    bool value = proSettings->value("ProcessSortOrder", true).toBool();
    proSettings->endGroup();

    return value;
}

QList<bool> SystemMonitor::getColumnHideFlags()
{
    proSettings->beginGroup("PROCESS");
    QString processColumns = proSettings->value("ProcessDisplayColumns", "name,user,status,cpu,pid,command,memory,priority").toString();
    proSettings->endGroup();

    if (processColumns.isEmpty()) {
        proSettings->beginGroup("PROCESS");
        processColumns = "name,user,status,cpu,pid,command,memory,priority";
        proSettings->setValue("ProcessDisplayColumns", processColumns);
        proSettings->endGroup();
        proSettings->sync();
    }

    QList<bool> toggleHideFlags;
    toggleHideFlags << processColumns.contains("name");
    toggleHideFlags << processColumns.contains("user");
    toggleHideFlags << processColumns.contains("status");
    toggleHideFlags << processColumns.contains("cpu");
    toggleHideFlags << processColumns.contains("pid");
    toggleHideFlags << processColumns.contains("command");
    toggleHideFlags << processColumns.contains("memory");
    toggleHideFlags << processColumns.contains("priority");

    return toggleHideFlags;
}

void SystemMonitor::moveCenter()
{
    QPoint pos = QCursor::pos();
    QRect primaryGeometry;
    for (QScreen *screen : qApp->screens()) {
        if (screen->geometry().contains(pos)) {
            primaryGeometry = screen->geometry();
        }
    }

    if (primaryGeometry.isEmpty()) {
        primaryGeometry = qApp->primaryScreen()->geometry();
    }

    this->move(primaryGeometry.x() + (primaryGeometry.width() - this->width())/2,
               primaryGeometry.y() + (primaryGeometry.height() - this->height())/2);
}

void SystemMonitor::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void SystemMonitor::moveDialog(QPoint diff)
{
#if QT_VERSION >= 0x050000
    // Move the window with some delay.
    // Seems to work better with Qt 5

    static QPoint d;
    static int count = 0;

    d += diff;
    count++;

    if (count > 3) {
        QPoint new_pos = pos() + d;
        if (new_pos.y() < 0) new_pos.setY(0);
        if (new_pos.x() < 0) new_pos.setX(0);
        move(new_pos);
        count = 0;
        d = QPoint(0,0);
    }
#else
    move(pos() + diff);
#endif
}

bool SystemMonitor::eventFilter(QObject *object, QEvent *event)
{
    if (object->objectName() == "SystemMonitorStack") {//让滚动条可以鼠标拖动
        drag_state = NOT_PDRAGGING;
        return false;
    }

    QEvent::Type type = event->type();

    if (type == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_F) {
            if (keyEvent->modifiers() == Qt::ControlModifier) {
                m_toolBar->setSearchEditFocus();
                return false;
            }
        }
    }

    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent) {
        return false;
    }

    if (mouseEvent->modifiers() != Qt::NoModifier) {
        drag_state = NOT_PDRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_PDRAGGING;
            return false;
        }

        drag_state = START_PDRAGGING;
        start_drag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (drag_state != PDRAGGING || mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_PDRAGGING;
            return false;
        }

        // Stop dragging and eat event
        drag_state = NOT_PDRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (drag_state == NOT_PDRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        drag_state = NOT_PDRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - start_drag;
    if (drag_state == START_PDRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        drag_state = PDRAGGING;
    }

    this->moveDialog(diff);

    start_drag = pos;
    event->accept();

    return true;
}

void SystemMonitor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPainterPath path;
    path.addRect(QRectF(rect()));
    painter.setOpacity(1);
    painter.fillPath(path, QColor("#FFFFFF"));
}
