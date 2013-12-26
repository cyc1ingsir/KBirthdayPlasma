#ifndef BLISTENTRY_H
#define BLISTENTRY_H

/* ************************************************
 *  @name: blistentry.h 
 *  @author: Meinhard Ritscher
 *  @date: 2008-10-21
 *
 *  $Id:  $
 *
 *  Description
 *  ============
 *  Data object storing all necessary info around
 *  the event
 *
 *  Histrory
 *  ============
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
 *   A copy of the license should be part of the package. If not, you can  *
 *   find it here: http://www.gnu.org/licenses/gpl.html                    *
 *                                                                         *
 ***************************************************************************/

#include <QString>
#include <QDate>

#include <KDebug>


class BListEntry{

    public:
        BListEntry(const QString &name, const QDate &date);
        ~BListEntry(){}

        QString name() const { return m_name; }
        unsigned int age() const{ return m_age;}
        int remainingDays() const{ return m_remainingDays;}
        void calculateDays();
        static bool lessThan(const BListEntry *a, const BListEntry *b);

    private:
    /** @brief The formatted name of that person. */
        QString m_name;
    /** @brief How old turns the person / the anniversary. */
        unsigned int m_age;
    /** @brief How many days until the event (this might be negative). */
        int m_remainingDays;

    /** @brief The date of the event*/
        QDate m_date;
};

#endif
