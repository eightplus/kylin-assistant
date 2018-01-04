#include "propertiesdialog.h"
#include "processworker.h"
#include "myimagebutton.h"
#include "util.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <QWidget>
#include <QFileInfo>
#include <QIcon>

PropertiesDialog::PropertiesDialog(QWidget *parent, pid_t processId) : QDialog(parent)
{
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint  | Qt::WindowCloseButtonHint);

    this->setAttribute(Qt::WA_TranslucentBackground);
//    this->setAttribute(Qt::WA_DeleteOnClose, false);
    this->setAttribute(Qt::WA_Resized, false);

    this->setMaximumSize(480, 600);
    this->setMinimumWidth(320);
    this->resize(380, 120);

    pid = processId;


    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    userLayout = new QHBoxLayout();
    userLayout->setContentsMargins(0, 0, 0, 0);
    nameLayout = new QHBoxLayout();
    nameLayout->setContentsMargins(0, 0, 0, 0);
    cmdlineLayout = new QHBoxLayout();
    cmdlineLayout->setContentsMargins(0, 0, 0, 0);
    cpuDurationLayout = new QHBoxLayout();
    cpuDurationLayout->setContentsMargins(0, 0, 0, 0);
    startTimeLayout = new QHBoxLayout();
    startTimeLayout->setContentsMargins(0, 0, 0, 0);

    closeButton = new MyImageButton();
    closeButton->setObjectName("CloseButton");
    connect(closeButton, &MyImageButton::clicked, this, [=] {
        this->close();
    });

    iconLabel = new QLabel();
    iconLabel->setFixedSize(96, 96);

    titleLabel = new QLabel();
    titleLabel->setStyleSheet("QLabel { background-color : transparent; font-size: 14px; font-weight: 500; color : #303030; }");

    userTitleLabel = new QLabel(QString("%1:").arg(tr("User name")));
    userTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    userTitleLabel->setFixedWidth(100);
    userTitleLabel->setAlignment(Qt::AlignRight);

    userLabel = new QLabel();
    userLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");

    userLayout->addWidget(userTitleLabel);
    userLayout->addWidget(userLabel);
    userLayout->addSpacing(20);

    nameTitleLabel = new QLabel(QString("%1:").arg(tr("Process name")));
    nameTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    nameTitleLabel->setFixedWidth(100);
    nameTitleLabel->setAlignment(Qt::AlignRight);

    nameLabel = new QLabel();
    nameLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");

    nameLayout->addWidget(nameTitleLabel);
    nameLayout->addWidget(nameLabel);
    nameLayout->addSpacing(20);

    cmdlineTitleLabel = new QLabel(QString("%1:").arg(tr("Command line")));
    cmdlineTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    cmdlineTitleLabel->setFixedWidth(100);
    cmdlineTitleLabel->setAlignment(Qt::AlignRight);

    cmdlineLabel = new QLabel();
    cmdlineLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");
    cmdlineLabel->setWordWrap(true);
    cmdlineLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    cmdlineLayout->addWidget(cmdlineTitleLabel);
    cmdlineLayout->addWidget(cmdlineLabel);
    cmdlineLayout->addSpacing(20);

    cpuDurationTitleLabel = new QLabel(QString("%1:").arg(tr("CPU Time")));
    cpuDurationTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    cpuDurationTitleLabel->setFixedWidth(100);
    cpuDurationTitleLabel->setAlignment(Qt::AlignRight);

    cpuDurationLabel = new QLabel();
    cpuDurationLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");
    cpuDurationLabel->setWordWrap(true);

    cpuDurationLayout->addWidget(cpuDurationTitleLabel);
    cpuDurationLayout->addWidget(cpuDurationLabel);
    cpuDurationLayout->addSpacing(20);

    startTimeTitleLabel = new QLabel(QString("%1:").arg(tr("Started Time")));
    startTimeTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    startTimeTitleLabel->setFixedWidth(100);
    startTimeTitleLabel->setAlignment(Qt::AlignRight);

    startTimeLabel = new QLabel();
    startTimeLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");
    startTimeLabel->setWordWrap(true);

    startTimeLayout->addWidget(startTimeTitleLabel);
    startTimeLayout->addWidget(startTimeLabel);
    startTimeLayout->addSpacing(20);

    layout->addWidget(closeButton, 0, Qt::AlignTop | Qt::AlignRight);
    layout->addSpacing(20);
    layout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(14);
    layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addLayout(userLayout);
    layout->addLayout(nameLayout);
    layout->addLayout(cmdlineLayout);
    layout->addLayout(cpuDurationLayout);
    layout->addLayout(startTimeLayout);
    layout->addSpacing(20);

    this->moveToCenter();

    this->initProcproperties();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refreshProcproperties()));
    timer->start(3000);
}

PropertiesDialog::~PropertiesDialog()
{
    if (timer != NULL) {
        disconnect(timer,SIGNAL(timeout()),this,SLOT(refreshProcproperties()));
        if(timer->isActive()) {
            timer->stop();
        }
        delete timer;
        timer = NULL;
    }

    delete closeButton;
    delete iconLabel;
    delete userTitleLabel;
    delete userLabel;
    delete nameTitleLabel;
    delete nameLabel;
    delete titleLabel;
    delete cmdlineTitleLabel;
    delete cpuDurationLabel;
    delete cpuDurationTitleLabel;
    delete startTimeLabel;
    delete startTimeTitleLabel;
    delete cmdlineLabel;
    delete userLayout;
    delete nameLayout;
    delete cmdlineLayout;
    delete cpuDurationLayout;
    delete startTimeLayout;
    delete layout;
}

void PropertiesDialog::initProcproperties()
{
    ProcessWorker *info;
    info = ProcessWorker::find(pid);
    if (info) {
        QString username = QString::fromStdString(info->user);
//        long nice = info->nice;
        QString name = QString::fromStdString(info->name);
//        QString status = formatProcessState(info->status);
//        uint cpu = info->pcpu;
//        long memory = info->mem;

        std::string desktopFile;
        desktopFile = getDesktopFileFromName(pid, name, "");

        QPixmap icon_pixmap;
        int iconSize = 96 * qApp->devicePixelRatio();
        QPixmap defaultPixmap = QIcon::fromTheme("application-x-executable").pixmap(iconSize, iconSize);
        if (desktopFile.size() == 0) {
            icon_pixmap = defaultPixmap;
            icon_pixmap.setDevicePixelRatio(qApp->devicePixelRatio());
        } else {
            icon_pixmap = getDesktopFileIcon(desktopFile, 96);
            if (icon_pixmap.isNull()) {
                icon_pixmap = defaultPixmap;
                icon_pixmap.setDevicePixelRatio(qApp->devicePixelRatio());
            }
            //QPixmap pixmap = QPixmap::fromImage(img).scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        QString displayName = getDisplayNameFromName(name, desktopFile);
        iconLabel->setPixmap(icon_pixmap);
        titleLabel->setText(displayName);
        userLabel->setText(username);
        nameLabel->setText(QString(info->name));
        cmdlineLabel->setText(QString(info->arguments));
        startTimeLabel->setText(QFileInfo(QString("/proc/%1").arg(pid)).created().toString("yyyy-MM-dd hh:mm:ss"));
        cpuDurationLabel->setText(formatDurationForDisplay(100 * info->cpu_time / info->frequency));
    }
}

void PropertiesDialog::refreshProcproperties()
{
    ProcessWorker *info;
    info = ProcessWorker::find(pid);
    if (info) {
        startTimeLabel->setText(QFileInfo(QString("/proc/%1").arg(pid)).created().toString("yyyy-MM-dd hh:mm:ss"));
        cpuDurationLabel->setText(formatDurationForDisplay(100 * info->cpu_time / info->frequency));
    }
}

pid_t PropertiesDialog::getPid()
{
    return pid;
}

QRect PropertiesDialog::getParentGeometry() const
{
    if (this->parentWidget()) {
        return this->parentWidget()->window()->geometry();
    } else {
        QPoint pos = QCursor::pos();

        for (QScreen *screen : qApp->screens()) {
            if (screen->geometry().contains(pos)) {
                return screen->geometry();
            }
        }
    }

    return qApp->primaryScreen()->geometry();
}

void PropertiesDialog::moveToCenter()
{
    QRect qr = geometry();
    qr.moveCenter(this->getParentGeometry().center());
    move(qr.topLeft());
}

void PropertiesDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->dragPosition = event->globalPos() - frameGeometry().topLeft();
        this->mousePressed = true;
    }

    QDialog::mousePressEvent(event);
}

void PropertiesDialog::mouseReleaseEvent(QMouseEvent *event)
{
    this->mousePressed = false;

    QDialog::mouseReleaseEvent(event);
}

void PropertiesDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (this->mousePressed) {
        move(event->globalPos() - this->dragPosition);
    }

    QDialog::mouseMoveEvent(event);
}

void PropertiesDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    //绘制圆角矩形
    painter.setPen(QPen(QColor("#0d87ca"), 0));//边框颜色   QColor(255, 255, 255, 153)
    painter.setBrush(QColor("#e9eef0"));//背景色   #0d87ca
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(1);
    QRectF r(0 / 2.0, 0 / 2.0, width() - 0, height() - 0);//左边 上边 右边 下边
    painter.drawRoundedRect(r, 4, 4);


    //绘制背景色
//    QPainterPath path;
//    path.addRect(QRectF(rect()));
//    painter.setOpacity(1);
//    painter.fillPath(path, QColor("#ffffff"));

    QDialog::paintEvent(event);
}

/*void PropertiesDialog::resizeEvent(QResizeEvent *event)
{
    if (event->size().width() >= maximumWidth()) {
        setFixedWidth(maximumWidth());
    }
    QDialog::resizeEvent(event);
}*/
