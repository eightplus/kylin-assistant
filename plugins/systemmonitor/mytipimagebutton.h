#ifndef MYTIPIMAGEBUTTON_H
#define MYTIPIMAGEBUTTON_H

#include <QEvent>
#include <QLabel>

#include "myimagebutton.h"

class MyTipImageButton : public MyImageButton
{
    Q_OBJECT

public:
    explicit MyTipImageButton(QWidget *parent = 0);

signals:
    void mouseLeave();

protected:
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    bool event(QEvent *e) Q_DECL_OVERRIDE;

private:
    void showTooltip(const QPoint &gPos);
};

#endif // MYTIPIMAGEBUTTON_H
