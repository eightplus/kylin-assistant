#ifndef RESOURCESINDICATOR_H
#define RESOURCESINDICATOR_H

#include <QObject>
#include <QWidget>
//#include <QLabel>
#include <QPixmap>

//class QVBoxLayout;
//class QHBoxLayout;

class ResourcesIndicator : public QWidget
{
    Q_OBJECT

    enum ResourcesState {Normal, Hover, Press, Checked};

public:
    ResourcesIndicator(int flag, QWidget * parent=0);
    ~ResourcesIndicator();

    void setChecked(bool flag);
    bool isChecked();
    void setTitle(const QString &title);
    void updatePercentAndInfo(double percent, const QString &info);
    ResourcesState getResourcesState() const;

signals:
    void clicked();

protected:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void updateBgColor();
    void setResourcesState(ResourcesState state);

private:
    ResourcesState m_state;
    bool m_isChecked;
    int m_currentPercent;
    QString m_title;
    QString m_info;
    int m_posX;
    int m_rectTopPadding;
    int m_rectTotalHeight;
    int m_rectTotalWidth;
    QColor m_outsideBorderColor;
    QColor m_bgColor;
    QColor m_borderColor;
//    QLabel *m_titleLabel = nullptr;
//    QLabel *m_textLabel = nullptr;
//    QVBoxLayout *m_rLayout = nullptr;
//    QHBoxLayout *m_layout = nullptr;
};

#endif // RESOURCESINDICATOR_H
