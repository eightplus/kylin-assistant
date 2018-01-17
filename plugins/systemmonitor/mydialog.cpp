#include "mydialog.h"
#include "myimagebutton.h"

#include <QLabel>
#include <QButtonGroup>
#include <QDebug>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QAction>
#include <QPushButton>
#include <QAbstractButton>
#include <QPainter>

MyDialog::MyDialog(const QString &title, const QString &message, QWidget *parent) :
    QDialog(parent)
   , mousePressed(false)
{
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint  | Qt::WindowCloseButtonHint);

    this->setAttribute(Qt::WA_TranslucentBackground);
//    this->setAttribute(Qt::WA_DeleteOnClose, false);
    this->setAttribute(Qt::WA_Resized, false);

    topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(20, 14, 20, 14);
    topLayout->setSpacing(20);

    titleLabel = new QLabel;
    titleLabel->setStyleSheet("QLabel{padding-top: 2px;padding-bottom: 2px;font-size: 12px;color: #000000;}");
    titleLabel->hide();
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    messageLabel = new QLabel;
    messageLabel->setStyleSheet("QLabel{padding-top: 2px;padding-bottom: 2px;font-size: 11px;color: #444444;}");
    messageLabel->hide();
    messageLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(5);
    textLayout->addStretch();
    textLayout->addWidget(titleLabel, 0, Qt::AlignLeft);
    textLayout->addWidget(messageLabel, 0, Qt::AlignLeft);
    textLayout->addStretch();

    contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addLayout(textLayout);

    topLayout->addLayout(contentLayout);


    closeButton = new MyImageButton(this);
    closeButton->setObjectName("CloseButton");
//    closeButton->setNormalPic(":/res/tool/close_normal.png");
//    closeButton->setHoverPic(":/res/tool/close_hover.png");
//    closeButton->setPressPic(":/res/tool/close_press.png");
    connect(closeButton, &MyImageButton::clicked, this, [=] {
        this->close();
    });
    closeButton->setAttribute(Qt::WA_NoMousePropagation);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(closeButton, 0, Qt::AlignTop | Qt::AlignRight);
    mainLayout->addLayout(topLayout);

    buttonLayout = new QHBoxLayout;
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(20, 14, 20, 14);
    mainLayout->addLayout(buttonLayout);

    QAction *button_action = new QAction(this);

    button_action->setShortcuts(QKeySequence::InsertParagraphSeparator);
    button_action->setAutoRepeat(false);

    QObject::connect(button_action, SIGNAL(triggered(bool)), this, SLOT(onDefaultButtonTriggered()));

    this->setLayout(mainLayout);
    this->addAction(button_action);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setFocus();

    setTitle(title);
    setMessage(message);

    this->moveToCenter();
}

MyDialog::~MyDialog()
{
    this->clearButtons();
}

void MyDialog::updateSize()
{
    if (!this->testAttribute(Qt::WA_Resized)) {
        QSize size = this->sizeHint();

        size.setWidth(qMax(size.width(), 440));
        size.setHeight(qMax(size.height(), 200));

        this->resize(size);
        this->setAttribute(Qt::WA_Resized, false);
    }
}

void MyDialog::onButtonClicked()
{
    QAbstractButton *button = qobject_cast<QAbstractButton*>(this->sender());

    if(button) {
        clickedButtonIndex = buttonList.indexOf(button);
        emit this->buttonClicked(clickedButtonIndex, button->text());
        this->done(clickedButtonIndex);//cancel:0   ok:1
    }
}

void MyDialog::onDefaultButtonTriggered()
{
    QAbstractButton *button = qobject_cast<QAbstractButton*>(this->focusWidget());

    if (button)
        button->click();
    else if (defaultButton)
        defaultButton->click();
}

int MyDialog::buttonCount() const
{
    return this->buttonList.count();
}

int MyDialog::addButton(const QString &text, bool isDefault/*, ButtonType type*/)
{
    int index = buttonCount();

    QAbstractButton *button = new QPushButton(text);
    button->setStyleSheet("QPushButton{font-size:12px;background:#ffffff;border:1px solid #bebebe;color:#000000;}QPushButton:hover{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;}QPushButton:pressed{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;}");
    button->setAttribute(Qt::WA_NoMousePropagation);
    button->setFixedHeight(24);

    QLabel* label = new QLabel;
    label->setStyleSheet("QLabel{background-color:rgba(0, 0, 0, 0.1);}");
    label->setFixedWidth(1);
    label->hide();

    if(index > 0 && index >= buttonCount()) {
        QLabel *label = qobject_cast<QLabel*>(this->buttonLayout->itemAt(this->buttonLayout->count() - 1)->widget());
        if(label)
            label->show();
    }

    this->buttonLayout->insertWidget(index * 2, button);
    this->buttonList << button;
    this->buttonLayout->insertWidget(index * 2 + 1, label);

    connect(button, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));

    if(isDefault) {
        setDefaultButton(button);
    }

    return index;
}

void MyDialog::clearButtons()
{
    this->buttonList.clear();

    while(this->buttonLayout->count()) {
        QLayoutItem *item = this->buttonLayout->takeAt(0);

        item->widget()->deleteLater();
        delete item;
    }
}

void MyDialog::setDefaultButton(QAbstractButton *button)
{
    this->defaultButton = button;
}

void MyDialog::setTitle(const QString &title)
{
    if (this->m_title == title)
        return;

    this->m_title = title;
    this->titleLabel->setText(title);
    this->titleLabel->setHidden(title.isEmpty());
}

void MyDialog::setMessage(const QString &message)
{
    if (this->m_message == message)
        return;

    this->m_message = message;
    this->messageLabel->setText(message);
    this->messageLabel->setHidden(message.isEmpty());
}

int MyDialog::exec()
{
    this->clickedButtonIndex = -1;

    int code = QDialog::exec();

    return this->clickedButtonIndex >= 0 ? this->clickedButtonIndex : code;
}

void MyDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    setAttribute(Qt::WA_Resized, false);
    this->updateSize();
}

void MyDialog::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);

    done(-1);
}

void MyDialog::childEvent(QChildEvent *event)
{
    QDialog::childEvent(event);

    if (event->added()) {
        if (this->closeButton) {
            this->closeButton->raise();
        }
    }
}

QRect MyDialog::getParentGeometry() const
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

void MyDialog::moveToCenter()
{
    QRect qr = geometry();
    qr.moveCenter(this->getParentGeometry().center());
    move(qr.topLeft());
}

void MyDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->dragPosition = event->globalPos() - frameGeometry().topLeft();
        this->mousePressed = true;
    }

    QDialog::mousePressEvent(event);
}

void MyDialog::mouseReleaseEvent(QMouseEvent *event)
{
    this->mousePressed = false;

    QDialog::mouseReleaseEvent(event);
}

void MyDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (this->mousePressed) {
        move(event->globalPos() - this->dragPosition);
    }

    QDialog::mouseMoveEvent(event);
}

void MyDialog::paintEvent(QPaintEvent *event)
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

void MyDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);

    this->titleLabel->setWordWrap(false);
    int labelMaxWidth = maximumWidth() - this->closeButton->width() - this->titleLabel->x();

    if (this->titleLabel->sizeHint().width() > labelMaxWidth) {
        this->titleLabel->setFixedWidth(labelMaxWidth);
        this->titleLabel->setWordWrap(true);
        this->titleLabel->setFixedHeight(this->titleLabel->sizeHint().height());
    }

    this->messageLabel->setWordWrap(false);
    labelMaxWidth = maximumWidth() - this->closeButton->width() - this->messageLabel->x();

    if (this->messageLabel->sizeHint().width() > labelMaxWidth) {
        this->messageLabel->setFixedWidth(labelMaxWidth);
        this->messageLabel->setWordWrap(true);
        this->messageLabel->setFixedHeight(this->messageLabel->sizeHint().height());
    }
}
