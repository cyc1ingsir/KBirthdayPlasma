#include "kbirthdayview.h"

/* ************************************************
 *  @name: kbirthdayview.cpp 
 *  @author: Meinhard Ritscher
 *
 *  $Id:  $
 *
 *  See header file for description and history
 *  This is more or less the code used from the
 *  device notifier plasmoid
 *
 * ********************************************** */
/*
    Copyright 2007 by Alexis Ménard <darktears31@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Qt

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QHeaderView>
#include <QtGui/QStandardItemModel>

//KDE
#include <KDebug>
#include <KIconLoader>
#include <KColorScheme>
#include <KGlobalSettings>

//Plasma
#include <Plasma/Delegate>

//using namespace KBirthday;

KBirthdayView::KBirthdayView(QWidget* parent)
    :QTreeView(parent)
{
    setRootIsDecorated(true);
    setHeaderHidden(true);
    setMouseTracking(true);
}


KBirthdayView::~KBirthdayView()
{

}

QModelIndex KBirthdayView::indexAt(const QPoint& point) const
{
    // simple linear search through the item rects, this will
    // be inefficient when the viewport is large
    QHashIterator<QModelIndex,QRect> iter(m_itemRects);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value().contains(point + QPoint(0, verticalOffset()))) {
            return iter.key();
        }
    }
    return QModelIndex();
}

void KBirthdayView::resizeEvent(QResizeEvent * event)
{
    //the columns after the first are squares KIconLoader::SizeMedium x KIconLoader::SizeMedium,
    //the first column takes all the remaining space
    calculateRects();

    if (header()->count() > 0) {
        const int newWidth = event->size().width() -
                             (header()->count()-1)*(sizeHintForRow(0));
        header()->resizeSection(0, newWidth);
    }
}

void KBirthdayView::mouseMoveEvent(QMouseEvent *event)
{
    const QModelIndex itemUnderMouse = indexAt(event->pos());
    if (itemUnderMouse != m_hoveredIndex && itemUnderMouse.isValid() &&
        state() == NoState) {
        update(itemUnderMouse);
        update(m_hoveredIndex);
        m_hoveredIndex = itemUnderMouse;
        setCurrentIndex(m_hoveredIndex);
    } else if (!itemUnderMouse.isValid()) {
        m_hoveredIndex = QModelIndex();
        setCurrentIndex(m_hoveredIndex);
    }

    QAbstractItemView::mouseMoveEvent(event);
}

void KBirthdayView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QAbstractItemView::mousePressEvent(event);
}

void KBirthdayView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (m_hoveredIndex.isValid()) {
        const QModelIndex oldHoveredIndex = m_hoveredIndex;
        m_hoveredIndex = QModelIndex();
        setCurrentIndex(m_hoveredIndex);
        update(oldHoveredIndex);
    }
}

QModelIndex KBirthdayView::moveCursor(CursorAction cursorAction,Qt::KeyboardModifiers modifiers )
{
    m_hoveredIndex = QModelIndex();

    return QTreeView::moveCursor(cursorAction, modifiers );
}

void KBirthdayView::calculateRects()
{
    if (!model()) {
        return;
    }

    m_itemRects.clear();
    int verticalOffset = TOP_OFFSET;

    const int rows = model()->rowCount(rootIndex());
    const int cols = header()->count();
    kDebug() << "painting" << rows << "rows" << cols << "columns";


    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const QModelIndex index = model()->index(i, j, rootIndex());
            if (model()->hasChildren(index)) {
                QRect itemRect(QPoint(HEADER_LEFT_MARGIN, verticalOffset),
                               QSize(width() - HEADER_LEFT_MARGIN, HEADER_HEIGHT));
                verticalOffset += itemRect.size().height();
                m_itemRects.insert(index, itemRect);

                QStandardItemModel * currentModel = dynamic_cast<QStandardItemModel *>(model());
                QStandardItem *currentItem = currentModel->itemFromIndex(index);
                // we display the children of this item
                for (int k = 0; k < currentItem->rowCount(); ++k) {
                    for (int l = 0; l < currentItem->columnCount(); ++l) {
                        QStandardItem *childItem = currentItem->child(k, l);
                        QModelIndex childIndex = childItem->index();
                        QRect itemChildRect;
                        if (l % 2 == 0) {
                            QSize size(width() - COLUMN_EJECT_SIZE,sizeHintForIndex(index).height());
                            itemChildRect = QRect(QPoint(HEADER_LEFT_MARGIN, verticalOffset), size);
                            m_itemRects.insert(childIndex, itemChildRect);
                            verticalOffset += itemChildRect.size().height();
                        } else {
                            QSize size(COLUMN_EJECT_SIZE - style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2,
                                       sizeHintForIndex(index).height());
                            itemChildRect = QRect(QPoint(width() - (COLUMN_EJECT_SIZE - COLUMN_EJECT_MARGIN ),
                                                  verticalOffset), size);
                            m_itemRects.insert(childIndex, itemChildRect);
                            verticalOffset += itemChildRect.size().height();
                        }
                    }
                }
            }
        }
    }
}

void KBirthdayView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!model()) {
        return;
    }

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    QHashIterator<QModelIndex, QRect> it(m_itemRects);
    while (it.hasNext()) {
        it.next();
        QRect itemRect = it.value();
        QRect rect(itemRect.x(), itemRect.y() - verticalOffset(), itemRect.width(), itemRect.height()); 
        if (event->region().contains(rect)) {
            QModelIndex index = it.key();
            if (model()->hasChildren(index)) {
                //kDebug()<<"header"<<itemRect;
                paintHeaderItem(painter, rect, index);
            } else {
                paintItem(painter, rect, index);
            }
        }
    }
}

void KBirthdayView::paintHeaderItem(QPainter &painter, const QRect &itemRect, const QModelIndex &index)
{
    QStyleOptionViewItem option = viewOptions();
    option.rect = itemRect;
    const int rightMargin = style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 16;
    const int dy = HEADER_TOP_MARGIN;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    QLinearGradient gradient(option.rect.topLeft(), option.rect.topRight());
    gradient.setColorAt(0.0, Qt::transparent);
    gradient.setColorAt(0.1, option.palette.midlight().color());
    gradient.setColorAt(0.5, option.palette.mid().color());
    gradient.setColorAt(0.9, option.palette.midlight().color());
    gradient.setColorAt(1.0, Qt::transparent);
    painter.setPen(QPen(gradient, 1));

    painter.drawLine(option.rect.x() + 6, option.rect.y() + dy + 2,
                     option.rect.right() - rightMargin , option.rect.y() + dy + 2);
    painter.setFont(KGlobalSettings::smallestReadableFont());
    painter.setPen(QPen(KColorScheme(QPalette::Active).foreground(KColorScheme::InactiveText), 0));
    QString text = index.data(Qt::DisplayRole).value<QString>();
    painter.drawText(option.rect.adjusted(0, dy, -rightMargin, 0),
                    Qt::AlignVCenter|Qt::AlignRight, text);
    painter.restore();
}

void KBirthdayView::paintItem(QPainter &painter, const QRect &itemRect, const QModelIndex &index)
{
    QStyleOptionViewItem option = viewOptions();
    option.rect = itemRect;

    if (selectionModel()->isSelected(index)) {
        option.state |= QStyle::State_Selected;
    }

    if (index == m_hoveredIndex) {
        option.state |= QStyle::State_MouseOver;
    }

    if (index == currentIndex()) {
        option.state |= QStyle::State_HasFocus;
    }

    itemDelegate(index)->paint(&painter,option,index);
}

#include "kbirthdayview.moc"
