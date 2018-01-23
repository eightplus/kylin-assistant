#ifndef MYSEARCHEDIT_H
#define MYSEARCHEDIT_H

#include <QFrame>
#include <QSize>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QLabel>

#include "mytristatebutton.h"

class MySearchEdit : public QFrame
{
    Q_OBJECT
public:
    explicit MySearchEdit(QWidget *parent = 0);
    ~MySearchEdit();

    void setPlaceHolder(const QString &text) {m_placeHolder->setText(text);}
    QSize sizeHint() const {return m_size;}
    QSize minimumSizeHint() const {return m_size;}
    const QString text() const;
    QLineEdit *getLineEdit() const;

public slots:
    void setEditFocus();
    void setText(const QString & text) {if (m_edit) m_edit->setText(text);}
    void clearEdit();

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
    QLabel *m_placeHolder;
    MyTristateButton *m_clearBtn;
    QPropertyAnimation *m_animation;
    QEasingCurve m_showCurve = QEasingCurve::OutCubic;
    QEasingCurve m_hideCurve = QEasingCurve::InCubic;
};

#endif // MYSEARCHEDIT_H
