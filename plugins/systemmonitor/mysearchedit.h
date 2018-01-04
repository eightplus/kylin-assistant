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

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool eventFilter(QObject *o, QEvent *e);

    inline void setAniDuration(const int duration) {m_animation->setDuration(duration);}
    inline void setAniShowCurve(const QEasingCurve curve) {m_showCurve = curve;}
    inline void setAniHideCurve(const QEasingCurve curve) {m_hideCurve = curve;}

    QLineEdit *getLineEdit() const;

public slots:
    void toEditMode();
    void setText(const QString & text) {if (m_edit) m_edit->setText(text);}
    inline void clear() {m_edit->clear();}

signals:
    void textChanged();
    void returnPressed();
    void editingFinished();
    void focusOut();
    void focusIn();

protected:
    void resizeEvent(QResizeEvent *e);
    bool event(QEvent *e);

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
