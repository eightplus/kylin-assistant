#include "myimagebutton.h"

#include <QMouseEvent>
#include <QEvent>


MyImageButton::MyImageButton(QWidget *parent)
    : QLabel(parent)
{
    setCheckable(false);//setCheckable(true);
    updateIcon();
}

MyImageButton::~MyImageButton()
{
}

void MyImageButton::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);

    if (!m_isChecked){
        setState(Hover);
    }

    event->accept();
    //QLabel::enterEvent(event);
}

void MyImageButton::leaveEvent(QEvent *event)
{
    if (!m_isChecked){
        setState(Normal);
    }

    event->accept();
    //QLabel::leaveEvent(event);
}

void MyImageButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    setState(Press);

    event->accept();
    //QLabel::mousePressEvent(event);
}

void MyImageButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (!rect().contains(event->pos()))
        return;

    if (m_isCheckable){
        m_isChecked = !m_isChecked;
        if (m_isChecked){
            setState(Checked);
        } else {
            setState(Normal);
        }
    } else {
        setState(Hover);
    }

    event->accept();
    //QLabel::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
        emit clicked();
}

void MyImageButton::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isCheckable && !rect().contains(event->pos())) {
        setState(Normal);
    }
}

void MyImageButton::updateIcon()
{
    switch (m_state) {
    case Hover:     if (!m_hoverPic.isEmpty()) setPixmap(QPixmap(m_hoverPic));      break;
    case Press:     if (!m_pressPic.isEmpty()) setPixmap(QPixmap(m_pressPic));      break;
    case Checked:   if (!m_checkedPic.isEmpty()) setPixmap(QPixmap(m_checkedPic));  break;
    default:        if (!m_normalPic.isEmpty()) setPixmap(QPixmap(m_normalPic));    break;
    }

    setAlignment(Qt::AlignCenter);

    emit stateChanged();
}

void MyImageButton::setState(MyImageButton::ButtonState state)
{
    if (m_state == state)
        return;

    m_state = state;

    updateIcon();
}

void MyImageButton::setCheckable(bool flag)
{
    m_isCheckable = flag;

    if (!m_isCheckable){
        setState(Normal);
    }
}

void MyImageButton::setChecked(bool flag)
{
    if (m_isCheckable == false){
        return;
    }

    m_isChecked = flag;
    if (m_isChecked){
        setState(Checked);
    } else {
        setState(Normal);
    }
}

bool MyImageButton::isChecked()
{
    return m_isChecked;
}

bool MyImageButton::isCheckable()
{
    return m_isCheckable;
}

void MyImageButton::setNormalPic(const QString &normalPicPixmap)
{
    m_normalPic = normalPicPixmap;
    updateIcon();
}

void MyImageButton::setHoverPic(const QString &hoverPicPixmap)
{
    m_hoverPic = hoverPicPixmap;
    updateIcon();
}

void MyImageButton::setPressPic(const QString &pressPicPixmap)
{
    m_pressPic = pressPicPixmap;
    updateIcon();
}

void MyImageButton::setCheckedPic(const QString &checkedPicPixmap)
{
    m_checkedPic = checkedPicPixmap;
    updateIcon();
}

MyImageButton::ButtonState MyImageButton::getButtonState() const
{
    return m_state;
}
