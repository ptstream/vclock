#include "alarmmanager.hpp"
#include "ui_alarmmanager.h"
#include "listwidgetitemmanager.hpp"
#include "../vtools/alarm.hpp"
#include "../vtools/helper.hpp"
#include <QScroller>
#include <QResizeEvent>
#include <QScrollBar>
#include <QLocale>
#include <QDir>

static int const maxRepeats = 10;

CAlarmManager::CAlarmManager (QList<CAlarm*>& alarms, QWidget* parent) : QDialog (parent),
                 ui (new Ui::CAlarmManager), m_alarms (alarms)
{
  setWindowFlags (windowFlags () & (~Qt::WindowContextHelpButtonHint));
  ui->setupUi (this);

  QTime time     = QTime::currentTime ();
  m_h            = time.hour () + 1;
  m_m            = time.minute ();
  m_alarmsBackup.reserve (alarms.size ());
  for (CAlarm* alarm : alarms)
  {
    alarm->stop (false);
    m_alarmsBackup.push_back (alarm->clone ());
  }

  loadHM (ui->m_hours, 24);
  loadHM (ui->m_minutes, 60);
  QDate date = QDate::currentDate ();

  if (touchScreenActivated ())
  {
    QListWidget* lws[] = { ui->m_hours, ui->m_minutes };
    for (QListWidget* lw : lws)
    {
      lw->setAttribute (Qt::WA_AcceptTouchEvents, true);
      lw->setVerticalScrollMode (QAbstractItemView::ScrollPerPixel);
      lw->setHorizontalScrollMode (QAbstractItemView::ScrollPerPixel);
      QScroller::grabGesture (lw, QScroller::TouchGesture);
    }
  }

  QScrollBar* sbs[] =  { ui->m_hours->verticalScrollBar (), ui->m_minutes->verticalScrollBar () };
  for (QScrollBar* sb : sbs)
  {
    sb->setPageStep (1);
    connect (sb, &QScrollBar::valueChanged, this, &CAlarmManager::scrollBarValueChanged);
  }

  updateAlarms ();
  QToolButton* btns[] = { ui->m_d1, ui->m_d2, ui->m_d3, ui->m_d4, ui->m_d5, ui->m_d6, ui->m_d7 };
  for (QToolButton* btn : btns)
  {
    QString text = btn->text ();
    text.truncate (2);
    btn->setText (text);
  }

  ui->m_repeat->setChecked (true);
  ui->m_calendar->setMinimumDate (date);
  ui->m_calendar->setSelectedDate (date);
  ui->m_calendar->setFocus ();
  ui->m_calendar->setHidden (true);
  ui->m_activate->setEnabled (false);
  updateAddRep ();
  m_timerID = startTimer (500);
}

CAlarmManager::~CAlarmManager ()
{
}

void CAlarmManager::centerCurrentTime (QListWidget* listWidget)
{
  int              row  = listWidget == ui->m_hours ? 12 * maxRepeats + m_h : 30 * maxRepeats + m_m;
  QListWidgetItem* item = listWidget->item (row);
  listWidget->scrollToItem (item, QAbstractItemView::PositionAtCenter);
  CListWidgetItemManager::remBold (listWidget);
  CListWidgetItemManager::setBold (item, true);
  updatedDateTime ();
}

void CAlarmManager::loadHM (QListWidget* listWidget, int count)
{
  listWidget->clear ();
  int cItems = count * maxRepeats;
  for (int iItem = 0; iItem < cItems; ++iItem)
  {
    QListWidgetItem* item = new QListWidgetItem (formatItem (iItem % count), listWidget);
    item->setTextAlignment (Qt::AlignHCenter);
  }
}

QString CAlarmManager::daysToString (CAlarm const * alarm) const
{
  QString      days;
  QToolButton* btns[] = { ui->m_d1, ui->m_d2, ui->m_d3, ui->m_d4, ui->m_d5, ui->m_d6, ui->m_d7 };
  for (unsigned iDay = 0; iDay < 7; ++iDay)
  {
    if (alarm->isEnabledFor (iDay + 1))
    {
      days += btns[iDay]->text ().mid (0, 2);
    }
  }

  return days;
}

void CAlarmManager::updateDayBtns (CAlarm const * alarm)
{
  QToolButton* btns[] = { ui->m_d1, ui->m_d2, ui->m_d3, ui->m_d4, ui->m_d5, ui->m_d6, ui->m_d7 };
  for (unsigned iDay = 0; iDay < sizeof (btns) / sizeof (QToolButton*); ++iDay)
  {
    btns[iDay]->setChecked (alarm->isEnabledFor (iDay + 1));
  }
}

void CAlarmManager::updateAlarms ()
{
  ui->m_alarms->clear ();
  if (!m_alarms.isEmpty ())
  {
    auto compare = [this] (CAlarm const * a1, CAlarm const * a2)
    {
      return a1->dateTime () < a2->dateTime ();
    };

    std::sort (m_alarms.begin (), m_alarms.end (), compare);

    int noNameIndex = 1;
    int count       = m_alarms.size ();
    for (int index = 0; index < count; ++index)
    {
      QDateTime const & dateTime = m_alarms[index]->dateTime ();
      QLocale           local    = QLocale::system ();
      QString           date     = local.toString (dateTime.date (), QLocale::ShortFormat);
      QString           time     = local.toString (dateTime.time (), "hh:mm");
      QString           days     = daysToString (m_alarms[index]);
      QString           name     = m_alarms[index]->name ();
      if (name.isEmpty ())
      {
        name = QString ("%1 %2").arg (tr ("Alarm")).arg (noNameIndex++);
      }

      QStringList texts;
      texts << date << time << name << days;
      QTreeWidgetItem* item = new QTreeWidgetItem (texts, index);
      ui->m_alarms->addTopLevelItem (item);
      item->setCheckState (0, m_alarms[index]->isEnabled () ? Qt::Checked : Qt::Unchecked);
    }

    ui->m_alarms->clearSelection ();
    QTreeWidgetItem* item = ui->m_alarms->topLevelItem (count - 1);
    item->setSelected (true);
  }
}

void CAlarmManager::loadFromItem (QTreeWidgetItem* item)
{
  int index = ui->m_alarms->indexOfTopLevelItem (item);
  if (index != -1)
  {
    ui->m_repeat->setChecked (m_alarms[index]->repeatCount () != 0);
    updateDayBtns (m_alarms[index]);
    QTime const time = m_alarms[index]->dateTime ().time ();
    m_h              = time.hour ();
    m_m              = time.minute ();
    centerCurrentTime (ui->m_hours);
    centerCurrentTime (ui->m_minutes);
    ui->m_calendar->setSelectedDate (m_alarms[index]->dateTime ().date ());
    ui->m_calendar->setFocus ();
  }
}

void CAlarmManager::resizeEvent (QResizeEvent* event)
{
  QDialog::resizeEvent (event);

  QScrollBar* sbh = ui->m_hours->verticalScrollBar ();
  QScrollBar* sbm = ui->m_minutes->verticalScrollBar ();
  sbh->blockSignals (true);
  sbm->blockSignals (true);
  centerCurrentTime (ui->m_hours);
  centerCurrentTime (ui->m_minutes);

  sbh->blockSignals (false);
  sbm->blockSignals (false);

  QHeaderView* header  = ui->m_alarms->header ();
  int          cCols   = ui->m_alarms->columnCount ();
  int          width   = ui->m_alarms->width ();
  int          size    = width / cCols;
  int          sizes[] = { size, size / 2, size, 3 * size / 2 };
  for (int k = 0; k < cCols; ++k)
  {
    header->resizeSection (k, sizes[k]);
  }
}

void CAlarmManager::updateAddRep ()
{
  bool none = !exists (m_dateTime) && m_dateTime > QDateTime::currentDateTime ();
  ui->m_add->setEnabled (none);
  ui->m_rep->setEnabled (!ui->m_alarms->selectedItems ().isEmpty ());
}

void CAlarmManager::updatedDateTime ()
{
  m_dateTime.setDate (ui->m_calendar->selectedDate ());
  m_dateTime.setTime (QTime (m_h, m_m));
  updateAddRep ();
}

void CAlarmManager::updateAlarm (CAlarm* alarm)
{
  if (!ui->m_repeat->isChecked ())
  {
    alarm->setRepeatCount (0);
  }

  alarm->setDateTime (m_dateTime);
  unsigned     days   = 0;
  QToolButton* btns[] = { ui->m_d1, ui->m_d2, ui->m_d3, ui->m_d4, ui->m_d5, ui->m_d6, ui->m_d7 };
  for (unsigned k = 0; k < sizeof (btns) / sizeof (QToolButton*); ++k)
  {
    if (btns[k]->isChecked ())
    {
      days |= 1 << k;
    }
  }

  alarm->setDays (days);
  alarm->setName (ui->m_name->text ());
  if (!ui->m_repeat->isChecked ())
  {
    alarm->setRepeatCount (0);
  }
}

bool CAlarmManager::exists (QDateTime const & dt) const
{
  QList<CAlarm*>::const_iterator itEnd = m_alarms.cend ();
  QList<CAlarm*>::const_iterator it    = std::find (m_alarms.cbegin (), itEnd, dt);
  return it != itEnd;
}

bool CAlarmManager::exists (CAlarm const & alarm) const
{
  bool found = false;
  for (CAlarm const * a : m_alarms)
  {
    if (*a == alarm)
    {
      found = true;
      break;
    }
  }

  return found;
}

void CAlarmManager::timerEvent (QTimerEvent* event)
{
  if (event->timerId () == m_timerID)
  {
    centerCurrentTime (ui->m_hours);
    centerCurrentTime (ui->m_minutes);
    killTimer (m_timerID);
  }
}

void CAlarmManager::scrollBarValueChanged ()
{
  QWidget* parent = static_cast<QWidget*>(sender ()->parent ());
  while (qobject_cast<QListWidget *>(parent) == nullptr)
  {
    parent = static_cast<QWidget*>(parent->parent ());
  }

  QListWidget* listWidget = static_cast<QListWidget*>(parent);
  int          hGrid      = listWidget->gridSize ().height ();
  QSize        size       = listWidget->viewport ()->size ();
  QPoint       loc (size.width () / 2, (size.height () - hGrid) / 2);
  QListWidgetItem* item = listWidget->itemAt (loc);
  if (item != nullptr)
  {
    CListWidgetItemManager::remBold (listWidget);
    CListWidgetItemManager::setBold (item, true);
    int* row;
    int modValue;
    if (listWidget == ui->m_minutes)
    {
      row      = &m_m;
      modValue = 60;
    }
    else
    {
      row      = &m_h;
      modValue = 24;
    }

    *row = listWidget->row (item) % modValue;
    updatedDateTime ();
  }
}

void CAlarmManager::on_m_hours_itemClicked (QListWidgetItem* item)
{
  m_h = ui->m_hours->row (item) % 24;
  centerCurrentTime (ui->m_hours);
}

void CAlarmManager::on_m_minutes_itemClicked (QListWidgetItem* item)
{
  m_m = ui->m_minutes->row (item) % 60;
  centerCurrentTime (ui->m_minutes);
}

void CAlarmManager::setActivateButton ()
{
  bool save = false;
  for (CAlarm const * alarm : m_alarms)
  {
    if (alarm->isEnabled ())
    {
      save = true;
      break;
    }
  }

  ui->m_activate->setText (save ? tr ("Save and activate") : tr ("Save"));
  ui->m_activate->setEnabled (isNotEqual (m_alarms, m_alarmsBackup));
}

void CAlarmManager::on_m_alarms_itemChanged (QTreeWidgetItem *item, int column)
{
  if (column == 0)
  {
    bool enable = item->checkState (column) == Qt::Checked;
    m_alarms[ui->m_alarms->indexOfTopLevelItem (item)]->setEnabled (enable);
    setActivateButton ();
  }
}

void CAlarmManager::on_m_alarms_itemSelectionChanged ()
{
  QList<QTreeWidgetItem*> selectedItems = ui->m_alarms->selectedItems ();
  bool                    enable        = !selectedItems.isEmpty ();
  if (enable)
  {
    QTreeWidgetItem* selectedItem = selectedItems.first ();
    loadFromItem (selectedItem);
  }

  ui->m_rem->setEnabled (enable);
}

void CAlarmManager::on_m_add_clicked ()
{
  CAlarm* alarm = new CAlarm;
  updateAlarm (alarm);
  m_alarms.push_back (alarm);
  updateAlarms ();
  ui->m_activate->setEnabled (isNotEqual (m_alarms, m_alarmsBackup));
}

void CAlarmManager::on_m_rep_clicked ()
{
  QList<QTreeWidgetItem*> selectedItems = ui->m_alarms->selectedItems ();
  if (!selectedItems.isEmpty ())
  {
    int index = ui->m_alarms->indexOfTopLevelItem (selectedItems.first ());
    if (index >= 0)
    {
      CAlarm* alarm = m_alarms[index];
      updateAlarm (alarm);
      updateAlarms ();
    }
  }

  ui->m_activate->setEnabled (isNotEqual (m_alarms, m_alarmsBackup));
}

bool isEqual (QList<CAlarm*> const & a1s, QList<CAlarm*> const & a2s)
{
  bool equal = a1s.size () == a2s.size ();
  if (equal)
  {
    int cEqual = 0;
    for (CAlarm const * a1 : a1s)
    {
      for (CAlarm const * a2 : a2s)
      {
        cEqual += *a1 == *a2 ? 1 : 0;
      }
    }

    equal = cEqual == a1s.size ();
  }

  return equal;
}

bool isNotEqual (QList<CAlarm*> const & a1s, QList<CAlarm*> const & a2s)
{
  return !isEqual (a1s, a2s);
}

void CAlarmManager::on_m_rem_clicked ()
{
  QList<QTreeWidgetItem*> selectedItems = ui->m_alarms->selectedItems ();
  if (!selectedItems.isEmpty ())
  {
    int index = ui->m_alarms->indexOfTopLevelItem (selectedItems.first ());
    delete m_alarms[index];
    m_alarms.removeAt (index);
    updateAlarms ();
  }

  updateAddRep ();
  ui->m_activate->setEnabled (isNotEqual (m_alarms, m_alarmsBackup));
  setActivateButton ();
}

void CAlarmManager::on_m_calendar_selectionChanged ()
{
  updatedDateTime ();
}

void CAlarmManager::on_m_showCalendar_clicked ()
{
  ui->m_calendar->setHidden (!ui->m_calendar->isHidden ());
  ui->m_calendar->setSelectedDate (QDate::currentDate ());
  ui->m_calendar->setFocus ();
}

void CAlarmManager::on_m_activate_clicked ()
{
  accept ();
}

void CAlarmManager::on_m_cancel_clicked ()
{
  for (CAlarm* alarm : m_alarms)
  {
    delete alarm;
  }

  m_alarms = m_alarmsBackup;
  reject ();
}
