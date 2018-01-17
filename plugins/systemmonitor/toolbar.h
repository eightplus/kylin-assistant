/*
 * Copyright (C) 2013 ~ 2018 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QFrame>
#include <QTimer>
#include <QSettings>

class QHBoxLayout;
class QLabel;
class QPushButton;
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
    void onCancelSearchBtnClicked(bool b);

signals:
    void changePage(int index);
    void searchSignal(QString searchContent);
    void canelSearchEditFocus();
    void activeWhoseProcessList(int index);

private:
    QSettings *proSettings;
    QColor m_topBorderColor;
    MySearchEdit *m_searchEdit;
    QPushButton *m_cancelSearchBtn;
    QString searchTextCache;
    QTimer *m_searchTimer = nullptr;
    QLabel *processLabel;
    ProcessCategory *processCategory;
    QHBoxLayout *m_layout;
    QHBoxLayout *m_lLayout;
    QHBoxLayout *m_mLayout;
    QHBoxLayout *m_rLayout;
};

#endif // TOOLBAR_H
