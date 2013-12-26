#ifndef KBIRTHDAY_APPLET_H
#define KBIRTHDAY_APPLET_H

/* ************************************************
 *  @name: addressbook.h 
 *  @author: Meinhard Ritscher
 *  @date: 2008-10-21
 *
 *  $Id:  $
 *
 *  Description
 *  ============
 *
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
 *   A copy of the license should be part of the package. If not you can   *
 *   find it here: http://www.gnu.org/licenses/gpl.html                    *
 *                                                                         *
 ***************************************************************************/

#include "ui_kbirthdayapplet_config.h"

//own
#include "kbirthdaydialog.h"

//Plasma
#include <Plasma/PopupApplet>
#include <Plasma/Svg>
#include <Plasma/DataEngine>

class QSizeF;
class BListEntry;

namespace Plasma{

    class Svg;

}

//namespace KBirthday
//{
    class KBirthdayApplet : public Plasma::PopupApplet
    {
    Q_OBJECT
    
    friend class KBirthdayDialog;
    
        public:
            KBirthdayApplet(QObject *parent, const QVariantList &args);
            ~KBirthdayApplet();

            void paintInterface(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                const QRect& contentsRect);
            /**
            * initialize the applet (called by plasma automatically)
            */
            void init();

            /**
            * The widget that displays the list of devices.
            */
            QWidget *widget();

        protected:
            void createConfigurationInterface(KConfigDialog *parent);
            void constraintsEvent(Plasma::Constraints constraints);
            void popupEvent(bool show);

        public slots:
            void dataUpdated(const QString &name, const Plasma::DataEngine::Data &data);
            /**
            * @internal Sets the tooltip content properly before showing.
            */
            void toolTipAboutToShow();
            /**
            * @internal Clears memory when needed.
            */
            void toolTipHidden();

        protected slots:
            void configAccepted();
            
        private:
            void updateEventList(const QList <QVariant> &anniversaries, QList<BListEntry*>** eventList);
            void updateEventCount(void);
            bool testThreshold(const int remainingDays);
            int printEvent(QPainter* p, int x, int y, int width, const BListEntry* entry);
            QString remainingDaysString(const int remainingDays);

        private:
        /** @brief The configuration dialog. */
            Ui::KBirthdayAppletConfig m_ui;

        /** @brief Various configurable settings. */
            QColor m_colour;
            QColor m_past_colour;
            QColor m_highlight_colour;

            int m_highlightTreshold;
            int m_eventTreshold;
            int m_pastTreshold;
            bool m_showAnniversaries;
        /** @brief  The dialog where events are displayed when in panel.*/
            KBirthdayDialog* m_pDialog;
        /** @brief  Set to true if plasmoid lives on a panel.*/
            bool m_isOnPanel;
        /** @brief A pointer to address book dataengine. */
            Plasma::DataEngine *m_pKabcEngine;
            Plasma::DataEngine *m_pAkonadiEngine;
        /** @brief A pointer to the list of birthdays. */
            QList<BListEntry*>* m_pBirthdays;
        /** @brief A pointer to the lsit of anniversaries. */
            QList<BListEntry*>* m_pAnniversaries;
        /** @brief number of outstanding events */
            int m_eventCount;
        /** @brief number of outstanding events */
            int m_birthEventCount;
        /** @brief number of outstanding events */
            int m_anniEventCount;
        /** @brief the icon used when the applet is in the taskbar */
            Plasma::IconWidget *m_pIcon;
        /** @brief Background when displayed in a panel */
            Plasma::Svg* m_svg;
    };
//} // namespace KBrithday

#endif
