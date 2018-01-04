#ifndef MYSEARCHEDIT_H
#define MYSEARCHEDIT_H

#include <QFrame>
#include <QSize>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QLabel>

#include "myimagebutton.h"

class MySearchEdit : public QFrame
{
    Q_OBJECT
public:
    explicit MySearchEdit(QWidget *parent = 0);
    ~MySearchEdit();

    void initInsideFrame();
    QSize sizeHint() const {return m_size;}
    QSize minimumSizeHint() const {return m_size;}
    const QString text() const;
    QLineEdit *getLineEdit() const;

public slots:
    void setEditFocus();
    void setText(const QString & text) {if (m_edit) m_edit->setText(text);}
    inline void clear() {m_edit->clear();}

signals:
    void textChanged();

protected:
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    QSize m_size;
    QLineEdit *m_edit;
    QLabel *m_searchBtn;
    MyImageButton *m_clearBtn;
    QFrame *m_insideFrame = NULL;
    QPropertyAnimation *m_animation;
    QEasingCurve m_showCurve = QEasingCurve::OutCubic;
    QEasingCurve m_hideCurve = QEasingCurve::InCubic;
};

#endif // MYSEARCHEDIT_H
