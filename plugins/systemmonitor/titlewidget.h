#ifndef MONITORTITLEWIDGET_H
#define MONITORTITLEWIDGET_H

#include <QFrame>
#include <QTimer>

class QHBoxLayout;

class TitleWidget : public QFrame
{
    Q_OBJECT
public:
    TitleWidget(QWidget *parent);
    ~TitleWidget();
    void initLeftContent();
    void initRightContent();
    void initWidgets();

protected:
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

signals:
    void updateMaxBtn();

private:
    QColor m_topBorderColor;
    QHBoxLayout *m_layout;
    QHBoxLayout *m_lLayout;
    QHBoxLayout *m_rLayout;
};

#endif // MONITORTITLEWIDGET_H
