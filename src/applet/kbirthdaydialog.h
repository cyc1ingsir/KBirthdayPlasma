#ifndef KBIRTHDAY_DIALOG_H
#define KBIRTHDAY_DIALOG_H

/* ************************************************
 *  @name: kbirthdaydialog.h 
 *  @author: Meinhard Ritscher
 *  @date: 2008-12-05
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

//Qt
#include <QWidget>

//Qt
class QLabel;
class QStandardItemModel;

//own
class KBirthdayApplet;


//namespace KBirthday
//{
  class KBirthdayView;

  /**
  * @short The dialog used to display events in a popup
  *
  */
  class KBirthdayDialog : public QObject
  {
  Q_OBJECT
      public:
          /**
          * Constructor of the dialog
          * @param notifier the birthday applet attached to this dialog
          * @param parent the parent of this object
          **/
          KBirthdayDialog(KBirthdayApplet* applet,QObject* parent = 0);
          
          /**
          * Default destructor
          **/
          virtual ~KBirthdayDialog();

         /**
          * Returns the related QWidget.
          **/
          QWidget* dialog();

          /**
          * Show the dialog
          **/
          void show();

          /**
           * Updates the event list
           **/
           void updateEventList();

      private:
         /**
          * @internal build the dialog
          **/
          void buildDialog();

      private:
          /** @brief The applet attached to this item */
          KBirthdayApplet* m_pApplet;
          /** @brief The widget which displays the events*/
          QWidget* m_pWidget;
          /** @brief QLabel which represent the title */
          QLabel* m_pLabel;
          /** @brief  */
          KBirthdayView *m_pEventView;
          /** @brief  */
          QStandardItemModel* m_pEventModel;
  };
  
#endif
