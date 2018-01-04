#include "toolbar.h"
#include "myimagebutton.h"
#include "mytipimagebutton.h"
#include "processcategory.h"
#include "mysearchedit.h"
#include "util.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleFactory>

ToolBar::ToolBar(QSettings *settings, QWidget *parent)
    :QFrame(parent)
    ,proSettings(settings)
{
    installEventFilter(this);
    setMouseTracking(true);
    setFixedHeight(TOP_TITLE_WIDGET_HEIGHT);

    m_topBorderColor = QColor(255, 255, 255, 153);
//    this->setAutoFillBackground(true);
//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor("#0d87ca"));
//    this->setPalette(palette);

    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    connect(searchTimer, SIGNAL(timeout()), this, SLOT(onRefreshSearchResult()));

    initWidgets();
}

ToolBar::~ToolBar()
{
    delete processLabel;
    delete processCategory;
    //Segmentation fault
    QLayoutItem *child;
    while ((child = m_lLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    while ((child = m_mLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    while ((child = m_rLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    delete m_layout;
}

bool ToolBar::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        if (obj == this) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape) {
                searchEdit->clear();
                emit pressEsc();
            }
        }
        else if (obj == searchEdit->getLineEdit()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Tab) {
                emit pressTab();
            }
        }
    }

    return QFrame::eventFilter(obj, event);
}

void ToolBar::focusInput()
{
    if (searchEdit->text() != "") {
        searchEdit->getLineEdit()->setFocus();
    } else {
        searchEdit->setFocus();
    }
}

void ToolBar::onRefreshSearchResult()
{
    if (searchEdit->text() == searchTextCache) {
        emit this->searchSignal(searchTextCache);
    }
}

void ToolBar::handleSearchTextChanged()
{
    searchTextCache = searchEdit->text();

    if (searchTimer->isActive()) {
        searchTimer->stop();
    }
    searchTimer->start(300);
}

void ToolBar::paintEvent(QPaintEvent *e)
{

    QFrame::paintEvent(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QColor tc(m_topBorderColor);
    int borderHeight = 1;
    QPainterPath tPath;
    tPath.moveTo(QPointF(x(), y() + borderHeight - 0.5));
    tPath.lineTo(QPointF(x() + width(), y() + borderHeight - 0.5));
    p.setPen(QPen(tc));
    p.drawPath(tPath);
}


void ToolBar::initLeftContent()
{
    QWidget *w = new QWidget;
    m_lLayout = new QHBoxLayout(w);
    m_lLayout->setContentsMargins(6, 0, 0, 0);
    m_lLayout->setSpacing(0);

    processLabel = new QLabel("");
    processLabel->setStyleSheet("QLabel { background-color : transparent; color : #505050; }");
    m_lLayout->addWidget(processLabel);

    m_layout->addWidget(w, 1, Qt::AlignLeft);
}

void ToolBar::initMiddleContent()
{
    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mLayout = new QHBoxLayout(w);
    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(0);

    MyTipImageButton *processButton = new MyTipImageButton();
    processButton->setCheckable(true);
    processButton->setChecked(true);
    processButton->setObjectName("ProcessBtn");

    MyTipImageButton *resourcesButton = new MyTipImageButton();
    resourcesButton->setCheckable(true);
    resourcesButton->setChecked(false);
    resourcesButton->setObjectName("ResourcesBtn");

    MyTipImageButton *disksButton = new MyTipImageButton();
    disksButton->setCheckable(true);
    disksButton->setChecked(false);
    disksButton->setObjectName("DisksBtn");

    connect(processButton, &MyTipImageButton::clicked, this, [=] {
        emit this->changePage(0);
        processButton->setChecked(true);
        resourcesButton->setChecked(false);
        disksButton->setChecked(false);
        if (!searchEdit->isVisible())
            searchEdit->setVisible(true);
    });
    connect(resourcesButton, &MyTipImageButton::clicked, this, [=] {
        emit this->changePage(1);
        processButton->setChecked(false);
        resourcesButton->setChecked(true);
        disksButton->setChecked(false);
        if (searchEdit->isVisible())
            searchEdit->setVisible(false);
    });
    connect(disksButton, &MyTipImageButton::clicked, this, [=] {
        emit this->changePage(2);
        processButton->setChecked(false);
        resourcesButton->setChecked(false);
        disksButton->setChecked(true);
        if (searchEdit->isVisible())
            searchEdit->setVisible(false);
    });
    processButton->setToolTip(tr("Processes"));
    resourcesButton->setToolTip(tr("Resources"));
    disksButton->setToolTip(tr("File Systems"));

    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(10);
    m_mLayout->addStretch();
    m_mLayout->addWidget(processButton);
    m_mLayout->addWidget(resourcesButton);
    m_mLayout->addWidget(disksButton);
    m_mLayout->addStretch();

    m_layout->addWidget(w);
}

void ToolBar::initRightContent()
{
    QWidget *w = new QWidget;
    m_rLayout = new QHBoxLayout(w);
    m_rLayout->setContentsMargins(0, 3, 6, 3);
    m_rLayout->setSpacing(0);

    connect(searchEdit, &MySearchEdit::textChanged, this, &ToolBar::handleSearchTextChanged, Qt::QueuedConnection);
    m_rLayout->addWidget(searchEdit);

    QString whose_processes = "user";
    proSettings->beginGroup("PROCESS");
    whose_processes = proSettings->value("WhoseProcesses", whose_processes).toString();
    proSettings->endGroup();
    int tabIndex = 1;
    if (whose_processes == "active") {
        tabIndex = 0;
    }
    else if (whose_processes == "all") {
        tabIndex = 2;
    }
    else {
        tabIndex = 1;
    }
    processCategory = new ProcessCategory(tabIndex);
    connect(processCategory, SIGNAL(activeWhoseProcessList(int)), this, SIGNAL(activeWhoseProcessList(int)));
    m_rLayout->addSpacing(10);
    m_rLayout->addWidget(processCategory, 0, Qt::AlignVCenter);
    m_layout->addWidget(w, 1, Qt::AlignRight);
}

void ToolBar::initWidgets()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    this->setLayout(m_layout);

    searchEdit = new MySearchEdit();
    searchEdit->setFixedWidth(222);
    searchEdit->getLineEdit()->installEventFilter(this);

    initLeftContent();
    initMiddleContent();
    initRightContent();
}
