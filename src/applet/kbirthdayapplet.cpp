#include "kbirthdayapplet.h"

/* ************************************************
 *  @name: kbirthdayapplet.cpp 
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

// Qt
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QPair>
#include <QTime>

// KDE
#include <KConfigDialog>
#include <KDebug>

// Plasma
#include <Plasma/Theme>
#include <Plasma/Svg>
#include <Plasma/IconWidget>
#if KDE_IS_VERSION(4,1,80)
#include <Plasma/ToolTipManager>
#endif

//own
#include "blistentry.h"

// linking this class to the desktop file so plasma can load it
K_EXPORT_PLASMA_APPLET(kbirthdayapplet, KBirthdayApplet)


typedef QPair<QString, QDate> KabEntry;
Q_DECLARE_METATYPE(KabEntry)


static const char *BIRTHDAY_SOURCE    = "Birthdays";
static const char *ANNIVERSARY_SOURCE = "Anniversaries";
static const char *UTC_SOURCE         = "UTC";
static const char *COLLECTION_SOURCE  = "ContactCollections";

KBirthdayApplet::KBirthdayApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_pDialog(0),
    m_isOnPanel(false),
    m_pKabcEngine(0),
    m_pAkonadiEngine(0),
    m_pBirthdays(0),
    m_pAnniversaries(0),
    m_eventCount(0),
    m_birthEventCount(0),
    m_anniEventCount(0),
    m_svg(0)
{
    kDebug() << "INIT init INIT init INIT";
    setBackgroundHints(DefaultBackground);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    resize(250, 300);
    
    // let's initialize the widget
    (void)widget();
    
    //m_pIcon = new Plasma::IconWidget(KIcon("cookie",NULL), QString());
    //setPopupIcon(m_pIcon->icon());
}

KBirthdayApplet::~KBirthdayApplet()
{
    if (hasFailedToLaunch()) {
        //Do some cleanup here
    } else {
    }
    delete m_pDialog;
    delete m_pBirthdays;
    delete m_pAnniversaries;
}

void KBirthdayApplet::popupEvent(bool show)
{
    if (show && m_isOnPanel) {
#if KDE_IS_VERSION(4,1,80)
        Plasma::ToolTipManager::self()->clearContent(this);
#endif
    }
}

void KBirthdayApplet::init()
{
    KConfigGroup configGroup = config();
    m_colour      = configGroup.readEntry("Color",                QColor(Qt::white));
    m_past_colour = configGroup.readEntry("Past Color",           QColor(150,150,150));
    m_highlight_colour = configGroup.readEntry("Highlight Color", QColor(170,0,0));
    m_highlightTreshold = configGroup.readEntry("Highlight Treshold", 2);
    m_eventTreshold = configGroup.readEntry("Event Treshold",       30);
    m_pastTreshold = configGroup.readEntry("Past Treshold",           2);
    m_showAnniversaries = configGroup.readEntry("Show Anniversaries", true);

    m_pKabcEngine = dataEngine("kabc");
    if (m_pKabcEngine) {
        m_pKabcEngine->connectSource(BIRTHDAY_SOURCE, this);
        m_pKabcEngine->connectSource(ANNIVERSARY_SOURCE, this);
    } else {
        //kDebug() << "Could not load kabc dataEngine";
        setFailedToLaunch(true, "Could not load kabc dataEngine");
    }
    Plasma::DataEngine* timeEngine = dataEngine("time");
    if (timeEngine) {
        timeEngine->connectSource("UTC", this, 360000, Plasma::AlignToHour);
    } else {
        kDebug() << "Warning: Could not load time dataEngine - no nightly update possible";
    }
    m_pAkonadiEngine = dataEngine("akonadi");
    if (m_pAkonadiEngine) {
        m_pAkonadiEngine->connectSource(COLLECTION_SOURCE, this);
    } else {
        kDebug() << "Could not load Akonadi dataEngine";
       // setFailedToLaunch(true, "Could not load kabc dataEngine");
    }

    m_svg = new Plasma::Svg(this);
    m_svg->setImagePath("widgets/birthdaycake");
    m_svg->setContainsMultipleImages(false);
 
}

QWidget *KBirthdayApplet::widget()
{
    if (!m_pDialog) {
        m_pDialog = new KBirthdayDialog(this);
        kDebug() << "KBirthdayDialog created";
    }
    return m_pDialog->dialog();
}

void KBirthdayApplet::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::FormFactorConstraint){
        Plasma::FormFactor formFctr = formFactor();
        if (formFctr == Plasma::Horizontal || formFctr == Plasma::Vertical) {
            setMinimumSize(QSize(42,42));
            setAspectRatioMode(Plasma::ConstrainedSquare);
#if KDE_IS_VERSION(4,1,80)
            Plasma::ToolTipManager::self()->registerWidget(this);
#endif
            setAspectRatioMode(Plasma::IgnoreAspectRatio);
            m_isOnPanel = true;
            kDebug() << "PANEL PANEL PANEL";

        } else if (formFctr == Plasma::MediaCenter || formFctr == Plasma::Planar) {
#if KDE_IS_VERSION(4,1,80)
            Plasma::ToolTipManager::self()->unregisterWidget(this);
#endif
            m_isOnPanel = false;
        }
    }
}

void KBirthdayApplet::paintInterface(QPainter *p,
    const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(option)

    const int left   = contentsRect.left();
    const int top    = contentsRect.top();
    const int width  = contentsRect.width();

    p->save();

    // if the widget lives in a panel, just display the number of comming events
    if (m_isOnPanel) {
    
        m_svg->resize(contentsRect.size());
        if (m_svg->hasElement("Brownie")) {
            m_svg->paint(p, contentsRect, "Brownie");
        } else {
            kDebug() << "Brownie not found in Svg. ...";
        }

        QFont font(p->font());
        font.setPointSize(font.pointSize() + 2);
        font.setBold(true);
        p->setFont(font);
        p->setPen(m_colour);

        p->drawText(contentsRect,Qt::AlignCenter|Qt::AlignVCenter,QString("%1").arg(m_eventCount));

        p->restore();
        return;
    }

    int entryHight = QFontMetrics(p->font()).height()*2;
    setMinimumSize(QSize((int)contentsRect.left()*2+250,
                    (int)contentsRect.top()*2+entryHight*m_eventCount-3 ));
    int top_offset = top+2;

    p->setPen(m_colour.darker());

    p->fillRect(left, top_offset, width, 1, m_colour.darker());
    top_offset += 2;

    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);

    if (   (!m_pBirthdays     || !m_birthEventCount) 
        && (!m_pAnniversaries || !m_anniEventCount))
    {
        p->drawText(left, top_offset, width, p->fontMetrics().height(),
                         Qt::AlignCenter, i18n("No Events"));
    } else {

        if (m_pBirthdays && m_birthEventCount) {
            p->drawText(left, top_offset, width, p->fontMetrics().height(),
                            Qt::AlignCenter, i18n("Birthdays"));
            top_offset += p->fontMetrics().height();
            foreach(const BListEntry *entry, *m_pBirthdays){
                top_offset = printEvent(p, left, top_offset, width, entry) + 3;
            }
        }
        if (m_showAnniversaries && m_anniEventCount) {
            p->fillRect(left, top_offset, width, 1, m_colour.darker());
            top_offset += 2;

            p->drawText(left, top_offset, width, p->fontMetrics().height(),
                            Qt::AlignCenter, i18n("Anniversaries"));
            top_offset += p->fontMetrics().height();
            foreach(const BListEntry *entry, *m_pAnniversaries){
                top_offset = printEvent(p, left, top_offset, width, entry) + 3;
            }
        }
    }
    p->restore();
}

void KBirthdayApplet::toolTipAboutToShow()
{
//    kDebug() << KDE::versionString() << "toolTipAboutToShow";
#if KDE_IS_VERSION(4,1,80)
// Tooltip handling seems to have changed a lot between 
// KDE version 4.1 and 4.2
    Plasma::ToolTipContent toolTip;
    toolTip.setMainText(i18n("Birthday Reminder"));
    if (m_eventCount > 0)
        toolTip.setSubText(i18np("One outstanding event", "%1 outstanding events", m_eventCount ));
    else
        toolTip.setSubText(i18n("No outstanding event"));
    Plasma::ToolTipManager::self()->setContent(this, toolTip);
#endif
}

void KBirthdayApplet::toolTipHidden()
{
#if KDE_IS_VERSION(4,1,80)
    Plasma::ToolTipManager::self()->clearContent(this);
#endif
}


void KBirthdayApplet::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *widget = new QWidget;

    m_ui.setupUi(widget);

    parent->setButtons(KDialog::Ok | KDialog::Cancel);
    parent->addPage(widget, parent->windowTitle(), icon());

    m_ui.colour->setColor(m_colour);
    m_ui.pastColour->setColor(m_past_colour);
    m_ui.highlightColour->setColor(m_highlight_colour);

    m_ui.spinEventTreshold->setValue(m_eventTreshold);
    m_ui.spinPast->setValue(m_pastTreshold);
    m_ui.spinHighlight->setValue(m_highlightTreshold);

    m_ui.showAnniversaries->setChecked(m_showAnniversaries);

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

/*
 * This method is called when the user clicks ok
 * in the config dialog
 *
 */

void KBirthdayApplet::configAccepted()
{

    m_colour            = m_ui.colour->color();
    m_past_colour       = m_ui.pastColour->color();
    m_highlight_colour  = m_ui.highlightColour->color();

    m_eventTreshold     = m_ui.spinEventTreshold->value();
    m_pastTreshold      = m_ui.spinPast->value();
    m_highlightTreshold = m_ui.spinHighlight->value();

    m_showAnniversaries = m_ui.showAnniversaries->isChecked();

    KConfigGroup configGroup = config();
    configGroup.writeEntry("Color",          m_colour);
    configGroup.writeEntry("Past Color", m_past_colour);
    configGroup.writeEntry("Highlight Color", m_highlight_colour);
    configGroup.writeEntry("Highlight Treshold",m_highlightTreshold);
    configGroup.writeEntry("Event Treshold",m_eventTreshold);
    configGroup.writeEntry("Past Treshold",m_pastTreshold);
    configGroup.writeEntry("Show Anniversaries",m_showAnniversaries);

    updateEventCount();
    update();

    emit configNeedsSaving();
}

void KBirthdayApplet::dataUpdated(const QString &name, const Plasma::DataEngine::Data &data)
{
    if (QString::compare(name, COLLECTION_SOURCE) == 0){
        kDebug() << "Collection updated";
    }else if (QString::compare(name, BIRTHDAY_SOURCE) == 0){
        updateEventList(data["Birthdays"].toList(), &m_pBirthdays);
        updateEventCount();
    }else if (QString::compare(name, ANNIVERSARY_SOURCE) == 0){
        updateEventList(data["Anniversaries"].toList(), &m_pAnniversaries);
        updateEventCount();
    }else if (QString::compare(name, UTC_SOURCE) == 0){
        // if midnight
        if ( QTime::currentTime().hour() == 0 ) {
	    if(m_pBirthdays){
	    //if started round midnight thees might not have
	    //created yet
            foreach(BListEntry *entry, *m_pBirthdays) {
                entry->calculateDays();
            }
	    }
	    if(m_pAnniversaries){
            foreach(BListEntry *entry, *m_pAnniversaries) {
                entry->calculateDays();
            }
        }
      }
    }
    update();
}

void KBirthdayApplet::updateEventList(const QList<QVariant> &list, QList<BListEntry*>** eventList)
{
    delete *eventList;
    *eventList = new QList<BListEntry*>();
    foreach(const QVariant &person, list){
        KabEntry ke = person.value<KabEntry>();
        BListEntry *entry = new BListEntry(ke.first, ke.second);
        (*eventList)->append(entry);
    }
    qSort((*eventList)->begin(), (*eventList)->end(), BListEntry::lessThan);
}

void KBirthdayApplet::updateEventCount()
{
    m_eventCount      = 0;
    m_birthEventCount = 0;
    m_anniEventCount  = 0;
    if (m_pBirthdays && ! m_pBirthdays->isEmpty()) {
        foreach(const BListEntry *entry, *m_pBirthdays) {
            if ( testThreshold(entry->remainingDays())) {
                    m_birthEventCount++;
                    // kDebug() << "Found birthday event" << entry->name();
            }
        }
    }
    if (m_pAnniversaries && m_showAnniversaries && ! m_pAnniversaries->isEmpty()) {
        foreach(const BListEntry *entry, *m_pAnniversaries){
            if ( testThreshold(entry->remainingDays())) {
                   m_anniEventCount++;
                   // kDebug() << "Found anniv event" << entry->name();
            }
        }
    }
    m_eventCount = m_birthEventCount + m_anniEventCount;
    // kDebug() << "EventCount now " << m_eventCount << " : " << m_birthEventCount << " : " <<  m_anniEventCount;
    
    if (m_pDialog)
      m_pDialog->updateEventList();
}

bool KBirthdayApplet::testThreshold(const int remainingDays)
{
  return ( (remainingDays > 0 && remainingDays <= m_eventTreshold) ||
          (remainingDays < 0 && remainingDays >= (m_pastTreshold* -1)) ||
                remainingDays == 0)?true:false;
}

int KBirthdayApplet::printEvent(QPainter* painter, int x, int y, int width, const BListEntry* entry)
{
    int remainingDays = entry->remainingDays();

    if ( !testThreshold(remainingDays))
        return y-3;

    painter->save();

    if (entry->remainingDays() > m_highlightTreshold) {
        painter->setPen(m_colour);
    } else if (remainingDays < 0) {
        painter->setPen(m_past_colour);
    } else {
        painter->setPen(m_highlight_colour);
    }

    painter->drawText(x, y, width, painter->fontMetrics().height(), Qt::AlignLeft,
                 QString("%1").arg(entry->name()));
    painter->drawText(x, y, width, painter->fontMetrics().height(), Qt::AlignRight,
                 QString("%1").arg(entry->age()));

    y += painter->fontMetrics().height();
    
    painter->drawText(x, y, width, painter->fontMetrics().height(), 
                      Qt::AlignRight, 
                      remainingDaysString(remainingDays));

    painter->restore();

    return y+painter->fontMetrics().height();
}

QString KBirthdayApplet::remainingDaysString(const int remainingDays)
{
    QString msg;
    if (remainingDays < -1) {
        msg = i18n("%1 days ago",remainingDays*-1);
    } else if (remainingDays == -1) {
        msg = i18n("yesterday");
    } else if (remainingDays == 0) {
        msg = i18n("today");
    } else if (remainingDays == 1) {
        msg = i18n("tomorrow");
    } else {
        msg = i18n("in %1 days",remainingDays);
    }
    return msg;
}

#include "kbirthdayapplet.moc"
