#include "memorywidget.h"

#include <QDebug>

MemoryWidget::MemoryWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(302, 165);

    circleRadius = 50; //半径
    rectWidth = 100; //文本框宽
    rectHeight = 20; // 文本框高
    center = rect().center();
    center.setX(center.x()/2); //内存圆心，X为窗口的1/4, Y为窗口的1/2
    swapcenter = rect().center();
    swapcenter.setX(swapcenter.x()*3/2); //SWAP圆心，X为窗口的3/4, Y为窗口的1/2
}

MemoryWidget::~MemoryWidget()
{

}


void MemoryWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing); //反锯齿

    QFont font = painter.font();
    font.setPointSize(22);
    font.setWeight(QFont::Light);
    painter.setFont(font);
    painter.setPen(QPen(QColor("#303030")));
    painter.drawText(QRect(rect().x() + 2, rect().y(), rect().width() - 4, rect().height()), Qt::AlignLeft | Qt::AlignTop, tr("Memory"));//绘制文本

    drawCircle(&painter);
    drawColorPie(&painter);
    drawText(&painter);
    painter.end();

}

void MemoryWidget::drawCircle(QPainter *painter)
{
    painter->save();

    //渐变效果设置
    QRadialGradient circleGradient(center, circleRadius, center);
    circleGradient.setColorAt(0.0, QColor(220, 220, 220));
    circleGradient.setColorAt(0.2, QColor(220, 220, 220));
    circleGradient.setColorAt(0.4, QColor(240, 240, 240));
    circleGradient.setColorAt(1.0, QColor(220, 220, 220));

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(circleGradient);
    //内存
    painter->drawEllipse(center, circleRadius, circleRadius);
    //SWAP
    painter->drawEllipse(swapcenter, circleRadius, circleRadius);

    painter->restore();
}

void MemoryWidget::drawColorPie(QPainter *painter)
{
    painter->save();

    painter->setPen(Qt::NoPen);
    //内存矩形，绘制扇形需要的参数，圆在矩形内且与矩形相切
    QPointF pieRectLeftTop(center.x()-circleRadius, center.y()-circleRadius);
    QPointF pieRectRightBottom(center.x()+circleRadius, center.y()+circleRadius);
    pieRect = QRectF(pieRectLeftTop, pieRectRightBottom);
    //SWAP矩形范围，，绘制扇形需要的参数，圆在矩形内且与矩形相切
    QPointF swapRectLeftTop(swapcenter.x()-circleRadius, swapcenter.y()-circleRadius);
    QPointF swapRectRightBottom(swapcenter.x()+circleRadius, swapcenter.y()+circleRadius);
    swappieRect = QRectF(swapRectLeftTop, swapRectRightBottom);

    painter->save();
    //颜色设置
    if(mi.percent > 0 && mi.percent < 50)
        painter->setBrush(QColor(0, 255, 0, 128)); //绿
    else if(mi.percent < 80)
        painter->setBrush(QColor(255, 255, 0, 128)); //黄
    else
        painter->setBrush(QColor(255, 0, 0, 128)); //红
    float currentPie = - (360 * (mi.percent/100)); //负数顺时针，0*16为3点钟位置

    painter->drawPie(pieRect, 90*16, currentPie*16); //绘制扇形，90*16为初始，12点钟位置
    painter->restore();

    if(mi.swappercent > 0 && mi.swappercent < 50)
        painter->setBrush(QColor(0, 255, 0, 128)); //绿
    else if(mi.swappercent < 80)
        painter->setBrush(QColor(255, 255, 0, 128)); //黄
    else
        painter->setBrush(QColor(255, 0, 0, 128)); //红
    float swapcurrentPie = - (360 * (mi.swappercent/100)); //负数顺时针

    painter->drawPie(swappieRect, 90*16, swapcurrentPie*16);//绘制扇形，90*16为初始，12点钟位置

    painter->restore();
}

void MemoryWidget::drawText(QPainter *painter)
{
    painter->save();

    //内存百分比矩形
    QPointF topLeft(center.x()-circleRadius, center.y()-circleRadius);
    QPointF bottomRight(center.x()+circleRadius, center.y()+circleRadius);
    QRectF textRect(topLeft, bottomRight);
    //内存状态矩形
    QPointF statustopLeft(center.x() - rectWidth, center.y()+circleRadius);
    QPointF statusbottomRight(center.x() + rectWidth, center.y()+circleRadius+rectHeight);
    QRectF statusRect(statustopLeft, statusbottomRight);
    //SWAP百分比矩形
    QPointF swaptopLeft(swapcenter.x()-circleRadius, swapcenter.y()-circleRadius);
    QPointF swapbottomRight(swapcenter.x()+circleRadius, swapcenter.y()+circleRadius);
    QRectF swaptextRect(swaptopLeft, swapbottomRight);
    //SWAP状态矩形
    QPointF swapstatustopLeft(swapcenter.x() - rectWidth, swapcenter.y() + circleRadius);
    QPointF swapstatusbottomRight(swapcenter.x() + rectWidth, center.y()+circleRadius+rectHeight);
    QRectF swapstatusRect(swapstatustopLeft, swapstatusbottomRight);

    //内存状态内容
    QString statusStr = tr("Used %1(%2), Total %3").arg(mi.user).arg(mi.percent).arg(mi.total);
    //SWAP状态内容
    QString swapstatusStr = tr("Used %1(%2), Total %3").arg(mi.swapused).arg(mi.swappercent).arg(mi.swaptotal);

    painter->save();

    //百分比字体设置
    QFont font;
    font.setPointSize(0.5*circleRadius);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::red);
    //绘制内存百分比
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignHCenter, QString::number(mi.percent, 'f', 1));
    //绘制SWAP百分比
    painter->drawText(swaptextRect, Qt::AlignVCenter | Qt::AlignHCenter, QString::number(mi.swappercent, 'f', 1));

    painter->restore();

    //状态字体设置
    QFont fontf;
    fontf.setPointSize(0.08*rectWidth);
    fontf.setBold(true);
    painter->setFont(fontf);
    //绘制内存状态
    painter->drawText(statusRect, Qt::AlignVCenter | Qt::AlignHCenter, statusStr);
    //绘制SWAP状态
    painter->drawText(swapstatusRect, Qt::AlignVCenter | Qt::AlignHCenter, swapstatusStr);

    painter->restore();
}

void MemoryWidget::onUpdateMemoryStatus()
{
    glibtop_mem mem;
    glibtop_swap swap;

    glibtop_get_mem(&mem);
    glibtop_get_swap(&swap);

    float swappercent = (swap.total ? (float)swap.used / (float)swap.total : 0.0f);
    float mempercent  = (float)mem.user  / (float)mem.total;

    mi.percent = mempercent * 100;
    mi.swappercent = swappercent * 100;

    //初始单位为字节，需要修正
    mi.user = mem.user / 1024 / 1024 /1024;
    mi.total = mem.total / 1024 / 1024 / 1024;

    mi.swapused = swap.used / 1024 / 1024 /1024;
    mi.swaptotal = swap.total / 1024 /1024 / 1024;

    repaint();
}
