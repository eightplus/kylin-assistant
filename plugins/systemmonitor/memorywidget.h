#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>

#include <QPainter>
#include <QImage>

#include <glibtop/mem.h>
#include <glibtop/swap.h>

typedef struct MemoryInfo
{
    float total;
    float free;
    float cached;
    float used;
    float user;
    float percent;

    float swaptotal;
    float swapfree;
    float swapcached;
    float swapused;
    float swappercent;

}Meminfo;

class MemoryWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryWidget(QWidget *parent = 0);
    ~MemoryWidget();

    void drawCircle(QPainter * painter);
    void drawColorPie(QPainter * painter);
    void drawText(QPainter * painter);

public slots:
    void onUpdateMemoryStatus();

protected:
    void paintEvent(QPaintEvent * event);

private:
    qreal circleRadius;
    qreal rectWidth;
    qreal rectHeight;
    qreal colorPieRadius;
    QPointF center;
    QPointF swapcenter;
    QRectF pieRect;
    QRectF swappieRect;
    Meminfo mi;
};

#endif // MEMORYWIDGET_H
