#ifndef DISKITEMLIST_H
#define DISKITEMLIST_H

#include <QFrame>
#include <QTimer>


class QVBoxLayout;

class DiskItem;

class DiskItemList : public QWidget
{
    Q_OBJECT

public:
    explicit DiskItemList(QFrame *parent = 0);
    explicit DiskItemList(const QString &title, QFrame *parent = 0);
    ~DiskItemList();

    DiskItem* getItem(int index);
    void appendItem(DiskItem * item);
    void removeItem(DiskItem * item);
    void moveItem(DiskItem *item, const int index);
    void setSpacing(const int spaceing);

    int itemCount() const;
    void clear();

private:
    bool eventFilter(QObject *, QEvent *event);
    void updateHeadTail();

private slots:
    void updateHeight();

private:
    QVBoxLayout *m_layout;
    QTimer *m_updateHTimer;
    QTimer *m_updateHeadTimer;
};

#endif // DISKITEMLIST_H
