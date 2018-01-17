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


#ifndef PROCESSLISTWIDGET_H
#define PROCESSLISTWIDGET_H

#include "processlistitem.h"

#include <QList>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

typedef bool (* SortFunction) (const ProcessListItem *item1, const ProcessListItem *item2, bool sort);
typedef bool (* SearchFunction) (const ProcessListItem *item, QString text);

class ProcessListWidget : public QWidget
{
    Q_OBJECT

public:

    ~ProcessListWidget();
    ProcessListWidget(QList<bool> columnShowOrHideFlags, QWidget *parent = 0);

    void setColumnSortingAlgorithms(QList<SortFunction> *list, int sortColumn=-1, bool sortOrder=false);
    void setSearchFunction(SearchFunction func);
    void addItems(QList<ProcessListItem*> items);
    void clearItems();
    void addSelectedItems(QList<ProcessListItem*> items, bool recordLastSelection=true);
    void clearSelectedItems(bool clearTheLast=true);
    void refreshItems(QList<ProcessListItem*> items);
    void doSearch(QString text);

    void selectAllItems();
    void selectFirstItem();
    void selectLastItem();
    void shiftSelectToEnd();
    void shiftSelectToHome();

    int getItemsTotalHeight();
    QList<ProcessListItem*> getSearchedItems(QList<ProcessListItem*> items);
    void sortItemsByColumn(int column, bool sortOrder);
    void selectPrevItem(int offset);
    void selectNextItem(int offset);
    void shiftSelectNextItem(int offset);
    void shiftSelectPrevItem(int offset);

    int getTopOffset();
    int getBottomOffset();
    int getScrollbarY();
    int getScrollbarH();
    int getScrollAreaH();

    QList<int> getTitleItemsWidths();
    void shiftSelectItemsWithBound(int startIndex, int endIndex);
    int setOffset(int offset);
    void startScrollbarHideTimer();
    bool mouseAtScrollArea(int x);
    bool mouseAtTitleArea(int y);

signals:
    void rightBtnClickedItems(QPoint pos, QList<ProcessListItem*> items);
    void changeColumnVisible(int index, bool visible, QList<bool> columnVisible);
    void changeSortStatus(int index, bool sortOrder);

public slots:
    void hideScrollbar();

protected:
    virtual void leaveEvent(QEvent *event);
    void keyPressEvent(QKeyEvent *keyEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *event);
    void paintScrollbar(QPainter *painter);

private:
    ProcessListItem *lastSelectItem;
    QList<ProcessListItem*> *m_listItems;
    QList<ProcessListItem*> *m_searchedItems;
    QList<ProcessListItem*> *m_selectedItems;
    QList<QString> columnTitles;
    QList<SortFunction> *m_sortFuncList;
    QList<bool> *m_sortOrderes;
    QList<int> m_columnWidths;
    QList<bool> m_columnVisibles;
    QString m_searchText;
    QTimer *m_hideScrollbarTimer = nullptr;
    SearchFunction m_searchFunc;
    bool m_defaultSortOrder;
    bool m_mouseAtScrollArea;
    bool m_mouseDragScrollbar;
    int m_defaultSortColumn;
    int m_origOffset;
    int m_offSet;
    int m_rowHeight;
    int m_scrollbarDragW;
    int m_scrollbarMinH;
    int m_arrowPadding;
    int m_titleHeight;
    int m_titleHoverColumn;
    int m_titlePadding;
    int m_titlePressColumn;
    QPixmap downArrowHoverPixmap;
    QPixmap downArrowNormalPixmap;
    QPixmap downArrowPressPixmap;
    QPixmap upArrowHoverPixmap;
    QPixmap upArrowNormalPixmap;
    QPixmap upArrowPressPixmap;
};

#endif // PROCESSLISTWIDGET_H
