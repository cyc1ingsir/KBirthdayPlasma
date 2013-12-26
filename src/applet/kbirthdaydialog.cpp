#include "kbirthdaydialog.h"

/* ************************************************
 *  @name: kbirthdaydialog.cpp 
 *  @author: Meinhard Ritscher
 *
 *  $Id:  $
 *
 *  See header file for description and history
 *
 * ********************************************** */
/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 or (at your option)    *
 *   any later version.                                                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY of FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   General Public Licencse for more details.                             *
 *                                                                         *
 *   A copy of the license should be part of the package. If not you can   *
 *   find it here: http://www.gnu.org/licenses/gpl.html                    *
 *                                                                         *
 ***************************************************************************/

//Qt
#include <QLabel>
#include <QVBoxLayout>
#include <QStandardItemModel>

//Plasma
#include <Plasma/Delegate>

//own
#include "kbirthdayapplet.h"
#include "kbirthdayview.h"
#include "blistentry.h"

using namespace Plasma;

KBirthdayDialog::KBirthdayDialog(KBirthdayApplet* applet,QObject *parent)
  :QObject((parent==0)?applet:parent),
  m_pApplet(applet),
  m_pWidget(0),
  m_pEventView(0),
  m_pEventModel(0)
{
    m_pEventModel = new QStandardItemModel(this);
    m_pEventModel->setItem(0, 1, NULL);
    m_pEventModel->setHeaderData(0, Qt::Horizontal,QString(""),Qt::EditRole);
    m_pEventModel->setHeaderData(1, Qt::Horizontal,QString(""),Qt::EditRole);

    buildDialog();
}

KBirthdayDialog::~KBirthdayDialog()
{
}

QWidget* KBirthdayDialog::dialog()
{
    return m_pWidget;
}

void KBirthdayDialog::buildDialog()
{
    m_pWidget = new QWidget();
    
    m_pLabel = new QLabel(m_pWidget);
    m_pLabel->setText(i18n("Outstanding Events"));
    
    QLabel *icon = new QLabel(m_pWidget);
    icon->setPixmap(KIcon("cookie").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium));
    
    m_pEventView = new KBirthdayView(m_pWidget);
    Plasma::Delegate* delegate = new Delegate(this);
    m_pEventView->setItemDelegate(delegate);
    m_pEventView->setModel(m_pEventModel);

  //heading
    QHBoxLayout *l_layout2 = new QHBoxLayout();
    l_layout2->setSpacing(0);
    l_layout2->setMargin(0);
    l_layout2->addWidget(icon);
    l_layout2->addWidget(m_pLabel);
    l_layout2->setAlignment(Qt::AlignCenter);
    
  //main layout
    QVBoxLayout* l_layout = new QVBoxLayout(m_pWidget);
    l_layout->setSpacing(0);
    l_layout->setMargin(0);
    l_layout->addLayout(l_layout2);
    l_layout->addWidget(m_pEventView);
    m_pWidget->setLayout(l_layout);
}

void KBirthdayDialog::updateEventList()
{
    m_pEventModel->clear();
    QStandardItem* rootItem = m_pEventModel->invisibleRootItem();
    
    

    if (m_pApplet->m_pBirthdays && ! m_pApplet->m_pBirthdays->isEmpty()) {
        QStandardItem* birthdaysItem = new QStandardItem(i18n("Birthdays"));
        rootItem->insertRow(0, birthdaysItem);
        int row = 0;
        foreach(const BListEntry* entry, *m_pApplet->m_pBirthdays) {
            if ( m_pApplet->testThreshold(entry->remainingDays())) {
                //QList<QStandardItem *> items;
                QStandardItem* item = new QStandardItem();
                item->setData(entry->name(), Qt::DisplayRole);
                item->setData(Plasma::Delegate::MainColumn, Plasma::Delegate::ColumnTypeRole);
                item->setData(true, Plasma::Delegate::SubTitleMandatoryRole);
                item->setData(m_pApplet->remainingDaysString(entry->remainingDays()),
                                Plasma::Delegate::SubTitleRole);
                //items << item;
                birthdaysItem->insertRow(0,item);
                item = new QStandardItem();
                item->setData(entry->age(), Qt::DisplayRole);
                //birthdaysItem->setChild(row,1,item);
                row++;
            }
        }
    }
    if (m_pApplet->m_pAnniversaries && 
        m_pApplet->m_showAnniversaries && 
        ! m_pApplet->m_pAnniversaries->isEmpty())
    {
        QStandardItem* anniversariesItem = new QStandardItem(i18n("Anniversaries"));
        rootItem->insertRow(1, anniversariesItem);
        foreach(const BListEntry* entry, *m_pApplet->m_pAnniversaries) {
            if ( m_pApplet->testThreshold(entry->remainingDays())) {
                QStandardItem* item = new QStandardItem();
                item->setData(entry->name(), Qt::DisplayRole);
                item->setData(Plasma::Delegate::MainColumn, Plasma::Delegate::ColumnTypeRole);
                item->setData(true, Plasma::Delegate::SubTitleMandatoryRole);
                item->setData(m_pApplet->remainingDaysString(entry->remainingDays()),
                                Plasma::Delegate::SubTitleRole);
                anniversariesItem->appendRow(item);
            }
        }
    }
    m_pEventView->calculateRects();
    kDebug() << "rows:" << rootItem->rowCount();
}

void KBirthdayDialog::show()
{
    kDebug() << "show called";
    m_pWidget->show();
}

#include "kbirthdaydialog.moc"
