#ifndef DISKITEM_H
#define DISKITEM_H

#include <QFrame>
#include <QMap>
#include <QMapNode>
#include <QLabel>

class MyImageButton;

class DiskItem : public QFrame
{
    Q_OBJECT

public:
    explicit DiskItem(QWidget *parent = 0);
    ~DiskItem();

    inline QString devName() const { return m_nameLabel->text(); }
    void setDevName(const QString &name);
    void setMountDir(const QString &mountdir);
    void setDiskType(const QString &disktype);
    void setTotalCapacity(const QString &totalcapacity);
    void setAvailCapacity(const QString &availcapacity);
    void setUsedCapactiy(const QString &usedcapactiy);
    void setPercentage(const QString &percentage);
    void setIsHead(bool head = true);
    void setIsTail(bool tail = true);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void clicked() const;
    void selected() const;
    void acceptNextPage() const;

private:
    QLabel *m_nameLabel;
    QLabel *m_mountLabel;
    QLabel *m_typeLabel;
    QLabel *m_totoalLabel;
    QLabel *m_availLabel;
    QLabel *m_usedLabel;
    QLabel *m_percentageLabel;
    MyImageButton *m_detailBtn;
    bool m_isHead;
    bool m_isTail;
};

#endif // DISKITEM_H

