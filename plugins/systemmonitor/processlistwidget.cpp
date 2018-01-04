#include "processlistwidget.h"
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QMenu>
#include <QStyleFactory>
#include <QWheelEvent>
#include <QPainter>
#include <QtMath>
#include <QPen>

ProcessListWidget::ProcessListWidget(QList<bool> columnShowOrHideFlags, QWidget *parent) : QWidget(parent)
  ,m_titlePadding(10)
  ,m_arrowPadding(2)
  ,m_titleHeight(36)
  ,m_rowHeight(36)
  ,m_offSet(0)
{
    this->m_searchFunc = NULL;
    this->m_searchText = "";
    this->m_titleHoverColumn = -1;
    this->m_titlePressColumn = -1;

    this->upArrowNormalPixmap = QPixmap(":/res/arrow_up_normal.png");
    this->upArrowHoverPixmap = QPixmap(":/res/arrow_up_hover.png");
    this->upArrowPressPixmap = QPixmap(":/res/arrow_up_press.png");
    this->downArrowNormalPixmap = QPixmap(":/res/arrow_down_normal.png");
    this->downArrowHoverPixmap = QPixmap(":/res/arrow_down_hover.png");
    this->downArrowPressPixmap = QPixmap(":/res/arrow_down_press.png");

    this->m_listItems = new QList<ProcessListItem*>();
    this->m_searchedItems = new QList<ProcessListItem*>();
    this->m_selectedItems = new QList<ProcessListItem*>();
    this->lastSelectItem = NULL;

    this->m_mouseAtScrollArea = false;
    this->m_mouseDragScrollbar = false;
    this->m_scrollbarDragW = 10;
    this->m_scrollbarMinH = 30;
    this->m_origOffset = 0;

    this->m_sortFuncList = new QList<SortFunction>();
    this->m_sortOrderes = new QList<bool>();

    this->columnTitles << tr("Process Name") << tr("User") << tr("Status") << tr("CPU") << tr("ID") << tr("Command Line") << tr("Memory") << tr("Priority");
    QList<int> widths;
    widths << 180 << 80 << 80 << 50 << 50 << -1 << 80 << 80;//-1时让改行填充所有剩余空间

    QFont font;
    font.setPointSize(9);//需要和填充所有剩余空间的那个的文字字体大小一致
    QFontMetrics fm(font);

    this->m_columnWidths.clear();
    for (int i = 0; i < widths.length(); i++) {
        if (widths[i] == -1) {
            this->m_columnWidths << widths[i];
        } else {//-1时让改行填充所有剩余空间
            int maxWidth = fm.width(this->columnTitles[i]) + this->m_titlePadding + upArrowNormalPixmap.width() / upArrowNormalPixmap.devicePixelRatio() + this->m_arrowPadding * 2;
            this->m_columnWidths << std::max(widths[i], maxWidth);
        }
    }

    this->m_columnVisibles.clear();
    for (int i = 0; i < columnShowOrHideFlags.count(); i++) {
        this->m_columnVisibles.append(columnShowOrHideFlags[i]);
    }

    QTimer::singleShot(0, this, SLOT(setFocus()));
}

ProcessListWidget::~ProcessListWidget()
{
    delete this->lastSelectItem;
    delete this->m_listItems;
    delete this->m_searchedItems;
    delete this->m_selectedItems;
    delete this->m_sortFuncList;
    delete this->m_sortOrderes;
    delete this->m_hideScrollbarTimer;
}

void ProcessListWidget::setColumnSortingAlgorithms(QList<SortFunction> *list, int sortColumn, bool sortOrder)
{
    this->m_sortFuncList = list;

    for (int i = 0; i < this->m_sortFuncList->count(); i++) {
        this->m_sortOrderes->append(false);
    }
    this->m_defaultSortColumn = sortColumn;
    this->m_defaultSortOrder = sortOrder;
}

void ProcessListWidget::setSearchFunction(SearchFunction func)
{
    this->m_searchFunc = func;
}

void ProcessListWidget::addItems(QList<ProcessListItem*> items)
{
    this->m_listItems->append(items);
    QList<ProcessListItem*> s_items = this->getSearchedItems(items);
    this->m_searchedItems->append(s_items);
    if (this->m_defaultSortColumn != -1) {
        this->sortItemsByColumn(this->m_defaultSortColumn, this->m_defaultSortOrder);
    }
}

void ProcessListWidget::clearItems()
{
    qDeleteAll(this->m_listItems->begin(), this->m_listItems->end());
    this->m_listItems->clear();
    this->m_searchedItems->clear();
}

void ProcessListWidget::addSelectedItems(QList<ProcessListItem*> items, bool recordLastSelection)
{
    this->m_selectedItems->append(items);

    if (recordLastSelection && this->m_selectedItems->count() > 0) {
        this->lastSelectItem = this->m_selectedItems->last();
    }
}

void ProcessListWidget::clearSelectedItems(bool clearTheLast)
{
    this->m_selectedItems->clear();
    if (clearTheLast) {
        this->lastSelectItem = NULL;
    }
}

void ProcessListWidget::refreshItems(QList<ProcessListItem*> items)
{
    QList<ProcessListItem*> *newSelection = new QList<ProcessListItem*>();
    ProcessListItem *newLastSelectionItem = NULL;

    for (ProcessListItem *item:items) {
        for (ProcessListItem *selectionItem:*this->m_selectedItems) {
            if (item->isSameItem(selectionItem)) {
                newSelection->append(item);
                break;
            }
        }
    }
    if (this->lastSelectItem != NULL) {
        for (ProcessListItem *item:items) {
            if (item->isSameItem(this->lastSelectItem)) {
                newLastSelectionItem = item;
                break;
            }
        }
    }

    clearItems();
    this->m_listItems->append(items);
    QList<ProcessListItem*> s_items = this->getSearchedItems(items);
    this->m_searchedItems->append(s_items);

    if (this->m_defaultSortColumn != -1) {
        this->sortItemsByColumn(this->m_defaultSortColumn, this->m_defaultSortOrder);
    }

    clearSelectedItems();
    addSelectedItems(*newSelection, false);
    this->lastSelectItem = newLastSelectionItem;
    this->m_offSet = setOffset(this->m_offSet);
    repaint();
}

void ProcessListWidget::doSearch(QString text)
{
    if (text == "" && this->m_searchText != text) {
        this->m_searchText = text;

        this->m_searchedItems->clear();
        this->m_searchedItems->append(*this->m_listItems);
    } else {
        this->m_searchText = text;
        QList<ProcessListItem*> s_items = this->getSearchedItems(*this->m_listItems);
        this->m_searchedItems->clear();
        this->m_searchedItems->append(s_items);
    }

    repaint();
}

void ProcessListWidget::selectAllItems()
{
    this->m_origOffset = this->m_offSet;

    clearSelectedItems();
    addSelectedItems(*this->m_searchedItems);

    this->m_offSet = this->getTopOffset();

    repaint();
}

void ProcessListWidget::selectFirstItem()
{
    this->m_origOffset = this->m_offSet;

    clearSelectedItems();

    QList<ProcessListItem*> items = QList<ProcessListItem*>();
    items << this->m_searchedItems->first();
    addSelectedItems(items);

    this->m_offSet = this->getTopOffset();

    repaint();
}

void ProcessListWidget::selectLastItem()
{
    this->m_origOffset = this->m_offSet;

    clearSelectedItems();

    QList<ProcessListItem*> items = QList<ProcessListItem*>();
    items << this->m_searchedItems->last();
    addSelectedItems(items);

    this->m_offSet = getBottomOffset();

    repaint();
}

void ProcessListWidget::shiftSelectToEnd()
{
    if (this->m_selectedItems->empty()) {
        selectLastItem();
    }
    else {
        int lastSelectionIndex = this->m_searchedItems->indexOf(this->lastSelectItem);
        shiftSelectItemsWithBound(lastSelectionIndex, this->m_searchedItems->count() - 1);
        this->m_offSet = getBottomOffset();
        repaint();
    }
}

void ProcessListWidget::shiftSelectToHome()
{
    if (this->m_selectedItems->empty()) {
        selectFirstItem();
    }
    else {
        int lastSelectionIndex = this->m_searchedItems->indexOf(this->lastSelectItem);
        shiftSelectItemsWithBound(0, lastSelectionIndex);
        this->m_offSet = this->getTopOffset();
        repaint();
    }
}

void ProcessListWidget::leaveEvent(QEvent * event){
    hideScrollbar();

    QWidget::leaveEvent(event);
}

void ProcessListWidget::hideScrollbar()
{
    this->m_mouseAtScrollArea = false;
    this->m_origOffset = this->m_offSet;

    repaint();
}

void ProcessListWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Home) {
        if (keyEvent->modifiers() == Qt::ControlModifier) {
            this->m_offSet = this->getTopOffset();
            repaint();
        } else if (keyEvent->modifiers() == Qt::ShiftModifier) {
            shiftSelectToHome();
        }else {
            selectFirstItem();
        }
    } else if (keyEvent->key() == Qt::Key_End) {
        if (keyEvent->modifiers() == Qt::ControlModifier) {
            this->m_offSet = getBottomOffset();
            repaint();
        } else if (keyEvent->modifiers() == Qt::ShiftModifier) {
            shiftSelectToEnd();
        } else {
            selectLastItem();
        }
    } else if (keyEvent->key() == Qt::Key_Up) {
        if (keyEvent->modifiers() == Qt::ShiftModifier) {
            shiftSelectPrevItem(1);
        } else {
            selectPrevItem(1);
        }
    } else if (keyEvent->key() == Qt::Key_Down) {
        if (keyEvent->modifiers() == Qt::ShiftModifier) {
            shiftSelectNextItem(1);
        } else {
            selectNextItem(1);
        }
    } else if (keyEvent->key() == Qt::Key_PageUp) {
        if (keyEvent->modifiers() == Qt::ControlModifier) {
            this->m_offSet = setOffset(this->m_offSet - getScrollAreaH());
            repaint();
        } else if (keyEvent->modifiers() == Qt::ShiftModifier) {
            shiftSelectPrevItem(getScrollAreaH() / this->m_rowHeight);
        } else {
            selectPrevItem(getScrollAreaH() / this->m_rowHeight);
        }
    } else if (keyEvent->key() == Qt::Key_PageDown) {
        if (keyEvent->modifiers() == Qt::ControlModifier) {
            this->m_offSet = setOffset(this->m_offSet + getScrollAreaH());
            repaint();
        } else if (keyEvent->modifiers() == Qt::ShiftModifier) {
            shiftSelectNextItem(getScrollAreaH() / this->m_rowHeight);
        } else {
            selectNextItem(getScrollAreaH() / this->m_rowHeight);
        }
    } else if (keyEvent->key() == Qt::Key_A) {
        if (keyEvent->modifiers() == Qt::ControlModifier) {
            selectAllItems();
        }
    }
}

void ProcessListWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (this->m_mouseDragScrollbar) {
        int barHeight = getScrollbarH();
        this->m_offSet = setOffset((mouseEvent->y() - barHeight / 2 - this->m_titleHeight) / (getScrollAreaH() * 1.0) * this->getItemsTotalHeight());
        repaint();
    }
    else if (mouseAtScrollArea(mouseEvent->x()) != this->m_mouseAtScrollArea) {
        this->m_mouseAtScrollArea = mouseAtScrollArea(mouseEvent->x());
        repaint();
    }
    else {
        bool isTitleArea = mouseAtTitleArea(mouseEvent->y());
        if (isTitleArea) {
            int hoverColumn = -1;
            if (this->m_sortFuncList->count() != 0 && this->m_sortFuncList->count() == this->columnTitles.count() && this->m_sortOrderes->count() == this->columnTitles.count()) {
                QList<int> titleItemsWidths = getTitleItemsWidths();

                int columnCounter = 0;
                int columnX = 0;
                for (int titleItemWidth:titleItemsWidths) {
                    if (titleItemWidth > 0) {
                        if (mouseEvent->x() > columnX && mouseEvent->x() < columnX + titleItemWidth) {
                            hoverColumn = columnCounter;
                            break;
                        }
                        columnX += titleItemWidth;
                    }
                    columnCounter++;
                }
            }
            if (hoverColumn != this->m_titleHoverColumn) {
                this->m_titleHoverColumn = hoverColumn;
                repaint();
            }
        }
    }
}

void ProcessListWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    setFocus();

    bool isTitleArea = mouseAtTitleArea(mouseEvent->y());
    bool isScrollArea = mouseAtScrollArea(mouseEvent->x());

    if (isTitleArea) {//点击列表的标题栏区域
        if (mouseEvent->button() == Qt::LeftButton) {
            if (this->m_sortFuncList->count() != 0 && this->m_sortFuncList->count() == this->columnTitles.count() && this->m_sortOrderes->count() == this->columnTitles.count()) {
                QList<int> titleItemsWidths = getTitleItemsWidths();
                int columnCounter = 0;
                int columnX = 0;
                for (int titleItemWidth : titleItemsWidths) {
                    if (titleItemWidth > 0) {
                        if (mouseEvent->x() > columnX && mouseEvent->x() < columnX + titleItemWidth) {
                            if (columnCounter != this->m_defaultSortColumn) {
                                (*this->m_sortOrderes)[columnCounter] = true;
                            }
                            else {
                                (*this->m_sortOrderes)[columnCounter] = !(*this->m_sortOrderes)[columnCounter];
                            }

                            this->m_defaultSortColumn = columnCounter;
                            this->m_defaultSortOrder = (*this->m_sortOrderes)[columnCounter];

                            emit this->changeSortStatus(this->m_defaultSortColumn, this->m_defaultSortOrder);

                            this->sortItemsByColumn(columnCounter, (*this->m_sortOrderes)[columnCounter]);

                            if (columnCounter != this->m_titlePressColumn) {
                                this->m_titlePressColumn = columnCounter;
                            }
                            repaint();
                            break;
                        }
                        columnX += titleItemWidth;
                    }
                    columnCounter++;
                }
            }
        } else if (mouseEvent->button() == Qt::RightButton) {
            if (m_columnVisibles.count() == this->columnTitles.count()) {
                QMenu *menu = new QMenu();
                for (int i = 0; i < m_columnVisibles.count(); i++) {
                    if (i != 0) {//让第一行总是显示，不可以设置显示或者不显示，其他行可以设置
                        QAction *action = new QAction(menu);
                        action->setText(this->columnTitles[i]);

                        action->setCheckable(true);
                        action->setChecked(m_columnVisibles[i]);
                        connect(action, &QAction::triggered, this, [this, action, i] {
                                m_columnVisibles[i] = !m_columnVisibles[i];
                                emit this->changeColumnVisible(i, m_columnVisibles[i], m_columnVisibles);
                                repaint();
                            });
                        menu->addAction(action);
                    }
                }

                menu->exec(this->mapToGlobal(mouseEvent->pos()));
            }
        }
    }
    else if (isScrollArea) {//点击滚动条区域
        int barHeight = getScrollbarH();
        int barY = getScrollbarY();

        if (mouseEvent->y() > barY && mouseEvent->y() < barY + barHeight) {
            this->m_mouseDragScrollbar = true;
        }
        else {
            this->m_offSet = setOffset((mouseEvent->y() - barHeight / 2 - this->m_titleHeight) / (getScrollAreaH() * 1.0) * this->getItemsTotalHeight());
            repaint();
        }
    }
    else {
        int pressItemIndex = (this->m_offSet + mouseEvent->y() - this->m_titleHeight) / this->m_rowHeight;
        if (pressItemIndex >= this->m_searchedItems->count()) {
            clearSelectedItems();

            repaint();
        } else {
            if (mouseEvent->button() == Qt::LeftButton) {
                if (pressItemIndex < this->m_searchedItems->count()) {
                    if (mouseEvent->modifiers() == Qt::ControlModifier) {
                        ProcessListItem *item = (*this->m_searchedItems)[pressItemIndex];

                        if (this->m_selectedItems->contains(item)) {
                            this->m_selectedItems->removeOne(item);
                        } else {
                            QList<ProcessListItem*> items = QList<ProcessListItem*>();
                            items << item;
                            addSelectedItems(items);
                        }
                    }
                    else if ((mouseEvent->modifiers() == Qt::ShiftModifier) && !this->m_selectedItems->empty()) {
                        int lastSelectionIndex = this->m_searchedItems->indexOf(this->lastSelectItem);
                        int selectionStartIndex = std::min(pressItemIndex, lastSelectionIndex);
                        int selectionEndIndex = std::max(pressItemIndex, lastSelectionIndex);

                        shiftSelectItemsWithBound(selectionStartIndex, selectionEndIndex);
                    }
                    else {
                        clearSelectedItems();

                        QList<ProcessListItem*> items = QList<ProcessListItem*>();
                        items << (*this->m_searchedItems)[pressItemIndex];
                        addSelectedItems(items);
                    }
                    repaint();
                }
            } else if (mouseEvent->button() == Qt::RightButton) {
                ProcessListItem *pressItem = (*this->m_searchedItems)[pressItemIndex];
                bool pressInSelectionArea = false;
                for (ProcessListItem *item : *this->m_selectedItems) {
                    if (item == pressItem) {
                        pressInSelectionArea = true;
                        break;
                    }
                }
                if (!pressInSelectionArea && pressItemIndex < this->m_searchedItems->length()) {
                    clearSelectedItems();
                    QList<ProcessListItem*> items = QList<ProcessListItem*>();
                    items << (*this->m_searchedItems)[pressItemIndex];
                    addSelectedItems(items);
                    repaint();
                }
                if (this->m_selectedItems->length() > 0) {
                    emit this->rightBtnClickedItems(this->mapToGlobal(mouseEvent->pos()), *this->m_selectedItems);
                }
            }
        }
    }
}

void ProcessListWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (this->m_mouseDragScrollbar) {
        this->m_mouseDragScrollbar = false;
        repaint();
    } else {
        if (this->m_titlePressColumn != -1) {
            this->m_titlePressColumn = -1;
            repaint();
        }
    }
}

void ProcessListWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical) {
        this->m_origOffset = this->m_offSet;
        qreal scrollStep = event->angleDelta().y() / 120.0;
        this->m_offSet = setOffset(this->m_offSet - scrollStep * this->m_rowHeight);
        repaint();
    }

    event->accept();
}

void ProcessListWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QList<int> titleItemsWidths = getTitleItemsWidths();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(0.05);

    int penWidth = 1;
    QPainterPath framePath;
    framePath.addRoundedRect(QRect(rect().x() + penWidth, rect().y() + penWidth, rect().width() - penWidth * 2, rect().height() - penWidth * 2), 4, 4);//背景弧度
    painter.setClipPath(framePath);

    //标题的背景
    if (this->m_titleHeight > 0) {
        QPainterPath titlePath;
        titlePath.addRect(QRectF(rect().x(), rect().y(), rect().width(), this->m_titleHeight));
        painter.setOpacity(0.02);
        painter.fillPath(titlePath, QColor("#ffffff"));
    }

    int title_Y = 0;
    int title_Height = 0;
    if (this->m_titleHeight > 0) {
        int columnCounter = 0;
        int columnX = 0;
        for (int itemWidth:titleItemsWidths) {
            if (itemWidth > 0) {
                painter.setOpacity(1);
                QFont font = painter.font();
                font.setPointSize(10);
                painter.setFont(font);
                //标题文字
                painter.setPen(QPen(QColor("#000000")));
                painter.drawText(QRect(columnX + this->m_titlePadding, 0, itemWidth, this->m_titleHeight), Qt::AlignVCenter | Qt::AlignLeft, this->columnTitles[columnCounter]);
                columnX += itemWidth;
                if (columnCounter < titleItemsWidths.size() - 1) {//垂直分割线
                    painter.setOpacity(0.1);
                    QPainterPath separatorPath;
                    separatorPath.addRect(QRectF(rect().x() + columnX - 1, rect().y() + 5, 1, this->m_titleHeight - 5*2));
                    painter.fillPath(separatorPath, QColor("#000000"));
                }

                //标题文字右侧的排序箭头图标
                if (this->m_defaultSortColumn == columnCounter) {
                    painter.setOpacity(1);
                    int arrowX = rect().x() + columnX - this->m_arrowPadding - upArrowNormalPixmap.width() / upArrowNormalPixmap.devicePixelRatio();
                    int arrowY = rect().y() + (this->m_titleHeight - downArrowNormalPixmap.height() / upArrowNormalPixmap.devicePixelRatio()) / 2;

                    if (this->m_defaultSortOrder) {
                        if (this->m_titlePressColumn == this->m_defaultSortColumn) {
                            painter.drawPixmap(QPoint(arrowX, arrowY), downArrowPressPixmap);
                        } else if (this->m_titleHoverColumn == this->m_defaultSortColumn) {
                            painter.drawPixmap(QPoint(arrowX, arrowY), downArrowHoverPixmap);
                        } else {
                            painter.drawPixmap(QPoint(arrowX, arrowY), downArrowNormalPixmap);
                        }
                    } else {
                        if (this->m_titlePressColumn == this->m_defaultSortColumn) {
                            painter.drawPixmap(QPoint(arrowX, arrowY), upArrowPressPixmap);
                        } else if (this->m_titleHoverColumn == this->m_defaultSortColumn) {
                            painter.drawPixmap(QPoint(arrowX, arrowY), upArrowHoverPixmap);
                        } else {
                            painter.drawPixmap(QPoint(arrowX, arrowY), upArrowNormalPixmap);
                        }
                    }
                }
            }
            columnCounter++;
        }

        title_Y += this->m_titleHeight;
        title_Height += this->m_titleHeight;
    }


    //去掉列表标题栏后的列表显示区域的背景
    painter.setOpacity(0.05);
    QPainterPath backgroundPath;
    backgroundPath.addRect(QRectF(rect().x(), rect().y() + this->m_titleHeight, rect().width(), rect().height() - this->m_titleHeight));
    painter.fillPath(backgroundPath, QColor("#ffffff"));

    //进程信息
    QPainterPath scrollAreaPath;
    scrollAreaPath.addRect(QRectF(rect().x(), rect().y() + this->m_titleHeight, rect().width(), getScrollAreaH()));

    int rowCounter = 0;
    for (ProcessListItem *item:*this->m_searchedItems) {
        if (rowCounter > ((this->m_offSet - this->m_rowHeight) / this->m_rowHeight)) {
            QPainterPath itemPath;
            itemPath.addRect(QRect(0, title_Y + rowCounter * this->m_rowHeight - this->m_offSet, rect().width(), this->m_rowHeight));
            painter.setClipPath((framePath.intersected(scrollAreaPath)).intersected(itemPath));

            bool isSelect = this->m_selectedItems->contains(item);
            painter.save();
            item->drawBackground(QRect(0, title_Y + rowCounter * this->m_rowHeight - this->m_offSet, rect().width(), this->m_rowHeight), &painter, rowCounter, isSelect);
            painter.restore();

            int columnCounter = 0;
            int columnTitleX = 0;
            for (int titleItemWidth : titleItemsWidths) {
                if (titleItemWidth > 0) {
                    painter.save();
                    item->drawForeground(QRect(columnTitleX, title_Y + rowCounter * this->m_rowHeight - this->m_offSet, titleItemWidth, this->m_rowHeight), &painter, columnCounter, rowCounter, isSelect);
                    painter.restore();
                    columnTitleX += titleItemWidth;
                }
                columnCounter++;
            }
            title_Height += this->m_rowHeight;
            if (title_Height > rect().height()) {
                break;
            }
        }
        rowCounter++;
    }
    painter.setClipPath(framePath);

    //背景
    QPen framePen;
    framePen.setColor(QColor("#F5F5F5"));
    painter.setOpacity(0.2);
    painter.drawPath(framePath);

    //垂直滚动条
    if (this->m_mouseAtScrollArea) {
        paintScrollbar(&painter);
    } else if (this->m_origOffset != this->m_offSet) {
        paintScrollbar(&painter);
        startScrollbarHideTimer();
    }
}

void ProcessListWidget::paintScrollbar(QPainter *painter)
{
    if (this->getItemsTotalHeight() > getScrollAreaH()) {
        qreal barOpacitry = 0;
        qreal barFrameOpacitry = 0;

        // Press.
        if (this->m_mouseDragScrollbar) {
            barOpacitry = 0.8;
            barFrameOpacitry = 0;
        } else {
            // Hover.
            if (this->m_mouseAtScrollArea) {
                barOpacitry = 0.7;
                barFrameOpacitry = 0;
            }
            // Normal.
            else {
                barOpacitry = 0.5;
                barFrameOpacitry = 0;
            }
        }

        int barWidth = (this->m_mouseAtScrollArea || this->m_mouseDragScrollbar) ? this->m_scrollbarDragW : 6;
        int barRadius = 3;

        int barY = getScrollbarY();
        int barHeight = getScrollbarH();

        painter->setOpacity(barOpacitry);
        QPainterPath path;
        path.addRoundedRect(
            QRectF(rect().x() + rect().width() - barWidth - 4,
                   barY + barRadius,
                   barWidth,
                   barHeight - barRadius * 2), barRadius, barRadius);
        painter->fillPath(path, QColor("#0B95D7"));

        QPen pen;
        pen.setColor(QColor("#0B95D7"));
        pen.setWidth(1);
        painter->setOpacity(barFrameOpacitry);
        painter->setPen(pen);
        painter->drawPath(path);
    }
}

void ProcessListWidget::selectNextItem(int offset)
{
    this->m_origOffset = this->m_offSet;

    if (this->m_selectedItems->empty()) {
        selectFirstItem();
    } else {
        int lastIndex = 0;
        for (ProcessListItem *item:*this->m_selectedItems) {
            int index = this->m_searchedItems->indexOf(item);
            if (index > lastIndex) {
                lastIndex = index;
            }
        }

        if (lastIndex != -1) {
            lastIndex = std::min(this->m_searchedItems->count() - 1, lastIndex + offset);

            clearSelectedItems(false);

            QList<ProcessListItem*> items = QList<ProcessListItem*>();
            items << (*this->m_searchedItems)[lastIndex];

            addSelectedItems(items);

            int itemIndex = lastIndex + 1;
            int itemOffset = setOffset(itemIndex * this->m_rowHeight - rect().height() + this->m_titleHeight);
            if (((this->m_offSet + getScrollAreaH()) / this->m_rowHeight) < itemIndex) {
                this->m_offSet = itemOffset;
            }

            repaint();
        }
    }
}

void ProcessListWidget::selectPrevItem(int offset)
{
    this->m_origOffset = this->m_offSet;

    if (this->m_selectedItems->empty()) {
        selectFirstItem();
    } else {
        int firstIndex = this->m_searchedItems->count();
        for (ProcessListItem *item:*this->m_selectedItems) {
            int index = this->m_searchedItems->indexOf(item);
            if (index < firstIndex) {
                firstIndex = index;
            }
        }

        if (firstIndex != -1) {
            firstIndex = std::max(0, firstIndex - offset);

            clearSelectedItems();

            QList<ProcessListItem*> items = QList<ProcessListItem*>();
            items << (*this->m_searchedItems)[firstIndex];

            addSelectedItems(items);

            int itemIndex = firstIndex - 1;
            int itemOffset = setOffset(itemIndex * this->m_rowHeight + this->m_titleHeight);
            if ((this->m_offSet / this->m_rowHeight) > itemIndex) {
                this->m_offSet = itemOffset;
            }
            repaint();
        }
    }
}

void ProcessListWidget::shiftSelectItemsWithBound(int startIndex, int endIndex)
{
    clearSelectedItems(false);
    QList<ProcessListItem*> items = QList<ProcessListItem*>();
    int index = 0;
    for (ProcessListItem *item:*this->m_searchedItems) {
        if (index >= startIndex && index <= endIndex) {
            items << item;
        }

        index++;
    }
    addSelectedItems(items, false);
}

void ProcessListWidget::shiftSelectPrevItem(int offset)
{
    this->m_origOffset = this->m_offSet;

    if (this->m_selectedItems->empty()) {
        selectFirstItem();
    } else {
        int firstIndex = this->m_searchedItems->count();
        int lastIndex = 0;
        for (ProcessListItem *item:*this->m_selectedItems) {
            int index = this->m_searchedItems->indexOf(item);

            if (index < firstIndex) {
                firstIndex = index;
            }

            if (index > lastIndex) {
                lastIndex = index;
            }
        }

        if (firstIndex != -1) {
            int lastSelectionIndex = this->m_searchedItems->indexOf(this->lastSelectItem);
            int selectionStartIndex, selectionEndIndex;

            if (lastIndex == lastSelectionIndex) {
                selectionStartIndex = std::max(0, firstIndex - offset);
                selectionEndIndex = lastSelectionIndex;
            } else {
                selectionStartIndex = firstIndex;
                selectionEndIndex = std::max(0, lastIndex - offset);
            }

            shiftSelectItemsWithBound(selectionStartIndex, selectionEndIndex);

            if (this->m_offSet / this->m_rowHeight >= selectionStartIndex) {
                this->m_offSet = setOffset((selectionStartIndex - 1) * this->m_rowHeight + this->m_titleHeight);
            }

            repaint();
        }
    }
}

void ProcessListWidget::shiftSelectNextItem(int offset)
{
    this->m_origOffset = this->m_offSet;

    if (this->m_selectedItems->empty()) {
        selectFirstItem();
    } else {
        int firstIndex = this->m_searchedItems->count();
        int lastIndex = 0;
        for (ProcessListItem *item:*this->m_selectedItems) {
            int index = this->m_searchedItems->indexOf(item);

            if (index < firstIndex) {
                firstIndex = index;
            }

            if (index > lastIndex) {
                lastIndex = index;
            }
        }

        if (firstIndex != -1) {
            int lastSelectionIndex = this->m_searchedItems->indexOf(this->lastSelectItem);
            int selectionStartIndex, selectionEndIndex;

            if (firstIndex == lastSelectionIndex) {
                selectionStartIndex = firstIndex;
                selectionEndIndex = std::min(this->m_searchedItems->count() - 1, lastIndex + offset);
            } else {
                selectionStartIndex = std::min(this->m_searchedItems->count() - 1, firstIndex + offset);
                selectionEndIndex = lastIndex;
            }

            shiftSelectItemsWithBound(selectionStartIndex, selectionEndIndex);

            if ((this->m_offSet + rect().height()) / this->m_rowHeight <= selectionEndIndex + 1) {
                this->m_offSet = setOffset((selectionEndIndex + 1) * this->m_rowHeight + this->m_titleHeight - rect().height());
            }


            repaint();
        }
    }
}

QList<int> ProcessListWidget::getTitleItemsWidths()
{
    QList<int> titleItemsWidths;
    if (this->m_columnWidths.length() > 0) {
        if (this->m_columnWidths.contains(-1)) {
            for (int i = 0; i < this->m_columnWidths.count(); i++) {
                if (this->m_columnWidths[i] != -1) {
                    if (m_columnVisibles[i]) {
                        titleItemsWidths << this->m_columnWidths[i];
                    } else {
                        titleItemsWidths << 0;
                    }
                } else {
                    if (m_columnVisibles[i]) {
                        int totalWidthOfOtherColumns = 0;

                        for (int j = 0; j < this->m_columnWidths.count(); j++) {
                            if (this->m_columnWidths[j] != -1 && m_columnVisibles[j]) {
                                totalWidthOfOtherColumns += this->m_columnWidths[j];
                            }
                        }

                        titleItemsWidths << rect().width() - totalWidthOfOtherColumns;
                    } else {
                        titleItemsWidths << 0;
                    }
                }
            }
        } else {
            for (int i = 0; i < this->m_columnWidths.count(); i++) {
                if (m_columnVisibles[i]) {
                    titleItemsWidths << this->m_columnWidths[i];
                } else {
                    titleItemsWidths << 0;
                }
            }
        }
    }
    else {
        titleItemsWidths << rect().width();
    }

    return titleItemsWidths;
}

bool ProcessListWidget::mouseAtScrollArea(int x)
{
    return (x > rect().x() + rect().width() - this->m_scrollbarDragW) && (x < rect().x() + rect().width());
}

bool ProcessListWidget::mouseAtTitleArea(int y)
{
    return (y > rect().y() && y < rect().y() + this->m_titleHeight);
}

int ProcessListWidget::setOffset(int offset)
{
    return std::max(0, std::min(offset, getBottomOffset()));
}

int ProcessListWidget::getItemsTotalHeight()
{
    return m_searchedItems->count() * m_rowHeight;
}

int ProcessListWidget::getScrollAreaH()
{
    return rect().height() - this->m_titleHeight;
}

int ProcessListWidget::getScrollbarY()
{
    return static_cast<int>((this->m_offSet / (this->getItemsTotalHeight() * 1.0)) * getScrollAreaH() + this->m_titleHeight);
}

int ProcessListWidget::getScrollbarH()
{
    return std::max(static_cast<int>(getScrollAreaH() / (this->getItemsTotalHeight() * 1.0) * rect().height()), this->m_scrollbarMinH);
}

int ProcessListWidget::getTopOffset()
{
    return 0;
}

QList<ProcessListItem*> ProcessListWidget::getSearchedItems(QList<ProcessListItem*> items)
{
    if (m_searchText == "" || m_searchFunc == NULL) {
        return items;
    } else {
        QList<ProcessListItem*> *search_items = new QList<ProcessListItem*>();

        for (ProcessListItem *item : items) {
            if (m_searchFunc(item, m_searchText)) {
                search_items->append(item);
            }
        }
        return *search_items;
    }
}

int ProcessListWidget::getBottomOffset()
{
    int itemsHeight = this->getItemsTotalHeight();
    if (itemsHeight > rect().height() - this->m_titleHeight) {
        return this->getItemsTotalHeight() - rect().height() + this->m_titleHeight;
    } else {
        return 0;
    }
}

void ProcessListWidget::sortItemsByColumn(int column, bool sortOrder)
{
    if (m_sortFuncList->count() != 0 && m_sortFuncList->count() == columnTitles.count() && m_sortOrderes->count() == columnTitles.count()) {
        qSort(m_searchedItems->begin(), m_searchedItems->end(), [&](const ProcessListItem *item1, const ProcessListItem *item2) {
                return (*m_sortFuncList)[column](item1, item2, sortOrder);
            });
    }
}

void ProcessListWidget::startScrollbarHideTimer()
{
    if (this->m_hideScrollbarTimer) {
        this->m_hideScrollbarTimer->stop();
    }

    this->m_hideScrollbarTimer = new QTimer();
    connect(this->m_hideScrollbarTimer, SIGNAL(timeout()), this, SLOT(hideScrollbar()));
    this->m_hideScrollbarTimer->start(2000);
}
