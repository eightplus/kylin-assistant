#include "mytipimagebutton.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QHBoxLayout>
#include <QTimer>
#include <QHelpEvent>

MyTipImageButton::MyTipImageButton(QWidget *parent)
    : MyImageButton(parent)
{

}

bool MyTipImageButton::event(QEvent *e)
{
    if (e->type() == QEvent::ToolTip) {
        if (QHelpEvent *he = static_cast<QHelpEvent *>(e)) {
            showTooltip(he->globalPos());

            return false;
        }
    }
    else if (e->type() == QEvent::Leave)  {
        emit mouseLeave();
        MyImageButton::leaveEvent(e);
    }
    else if (e->type() == QEvent::MouseButtonPress) {
        emit mouseLeave();

    }

    return MyImageButton::event(e);
}

void MyTipImageButton::enterEvent(QEvent *e)
{
    if (isEnabled()) {
        MyImageButton::enterEvent(e);
    }
}

void MyTipImageButton::showTooltip(const QPoint &gPos)
{
    if (toolTip().trimmed().isEmpty()) {
        return;
    }

    QFrame *tf = new QFrame();
    tf->setStyleSheet(this->styleSheet());
    tf->setWindowFlags(Qt::ToolTip);
    tf->setAttribute(Qt::WA_TranslucentBackground);
    QLabel *tl = new QLabel(tf);
    tl->setObjectName("TooltipLabel");
    tl->setText(toolTip());
    QHBoxLayout *layout = new QHBoxLayout(tf);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(tl);

    tf->show();
    QRect dr = qApp->desktop()->geometry();
    int y = gPos.y() + tf->height();
    if (y > dr.y() + dr.height()) {
        y = gPos.y() - tf->height() - 10;
    }
    tf->move(gPos.x() - tf->width()/3, y - tf->height()/3);

    QTimer::singleShot(5000, tf, SLOT(deleteLater()));

    connect(this, &MyTipImageButton::mouseLeave, tf, &QFrame::deleteLater);
}
