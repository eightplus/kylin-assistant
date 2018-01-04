#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QFrame>
#include <QTimer>
#include <QSettings>

class QHBoxLayout;
class QLabel;
class ProcessCategory;
class MySearchEdit;

class ToolBar : public QFrame
{
    Q_OBJECT
public:
    ToolBar(QSettings *settings, QWidget *parent);
    ~ToolBar();

    void setSearchEditFocus();
    void initLeftContent();
    void initMiddleContent();
    void initRightContent();
    void initWidgets();

protected:
    void paintEvent(QPaintEvent *e) override;
    bool eventFilter(QObject *, QEvent *event);

public slots:
    void onRefreshSearchResult();
    void handleSearchTextChanged();

signals:
    void changePage(int index);
    void searchSignal(QString searchContent);
    void canelSearchEditFocus();
    void activeWhoseProcessList(int index);

private:
    QSettings *proSettings;
    QColor m_topBorderColor;
    MySearchEdit *searchEdit;
    QString searchTextCache;
    QTimer *searchTimer;
    QLabel *processLabel;
    ProcessCategory *processCategory;
    QHBoxLayout *m_layout;
    QHBoxLayout *m_lLayout;
    QHBoxLayout *m_mLayout;
    QHBoxLayout *m_rLayout;
};

#endif // TOOLBAR_H
