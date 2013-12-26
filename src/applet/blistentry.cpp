#include "blistentry.h"

/* ************************************************
 *  @name: blistentry.cpp 
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


BListEntry::BListEntry(const QString &name, const QDate &date)
    :m_name(name)
{
    m_date = date;
    calculateDays();
}

void BListEntry::calculateDays()
{
    QDate today = QDate::currentDate();
//    today = QDate(2001,2,2);
    int year = today.year();
    int month = today.month();

// every event from the past six months
// are calculated in the past, events of the next
// six months will be in the future
    int deltaMonth = month - m_date.month();

    // if the year has turned in the past six or
    // will turn in the next six months, we have
    // to adust the year
    if(deltaMonth < -5){
        year--;
    }else if(deltaMonth > 5){
        year++;
    }
    m_age = year - m_date.year();
    QDate tempDate = QDate(year, m_date.month(), m_date.day());
    m_remainingDays = today.daysTo(tempDate);
//    kDebug() << m_name << " - " << m_remainingDays;
}

bool BListEntry::lessThan(const BListEntry *a, const BListEntry *b)
{
    return ( a->remainingDays() == b->remainingDays())?
                (a->age() < b->age()):(a->remainingDays() < b->remainingDays());
}
