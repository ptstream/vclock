#include "timermanager.hpp"
#include "ui_timermanager.h"
#include "listwidgetitemmanager.hpp"
#include "../vmode/timer.hpp"
#include "../vtools/helper.hpp"
#include <QScroller>
#include <QScrollBar>

static int const maxRepeats = 10;

CTimerManager::CTimerManager (QList<CTimer*>& timers, QWidget* parent) :
            QDialog (parent), ui (new Ui::CTimerManager), m_timers (timers)
{
  setWindowFlags (windowFlags () & (~Qt::WindowContextHelpButtonHint));
  ui->setupUi (this);
  m_timersBackup.reserve (timers.size ());
  for (CTimer* timer : m_timers)
  {
    CTimer* clone = timer->clone ();
    m_timersBackup.push_back (clone);
  }

  QListWidget*                lws[]       = { ui->m_s, ui->m_m };
  int                         mod[]       = { 60, 100 };
  bool                        touchScreen = touchScreenActivated ();
  for (int i = 0, count = sizeof (mod) / sizeof (int); i < count; ++i)
  {
    for (int iItem = 0, cItems = mod[i] * maxRepeats; iItem < cItems; ++iItem)
    {
      QListWidgetItem* item = new QListWidgetItem (formatItem (iItem % mod[i]), lws[i]);
      item->setTextAlignment (Qt::AlignHCenter);
    }

    if (touchScreen)
    {
      lws[i]->setAttribute (Qt::WA_AcceptTouchEvents, true);
      lws[i]->setVerticalScrollMode (QAbstractItemView::ScrollPerPixel);
      lws[i]->setHorizontalScrollMode (QAbstractItemView::ScrollPerPixel);
      QScroller::grabGesture (lws[i], QScroller::TouchGesture);
    }
  }

  QScrollBar* sbs[] = { ui->m_s->verticalScrollBar (), ui->m_m->verticalScrollBar () };
  for (QScrollBar* sb : sbs)
  {
    sb->setPageStep (1);
    connect (sb, &QScrollBar::valueChanged, this, &CTimerManager::scrollBarValueChanged);
  }

  updateTimers ();
  connect (ui->m_s, &QListWidget::itemClicked, this, &CTimerManager::secondItemClicked);
  connect (ui->m_m, &QListWidget::itemClicked, this, &CTimerManager::minuteItemClicked);
  connect (ui->m_start, &QPushButton::clicked, this, &CTimerManager::start);
  connect (ui->m_cancel, &QPushButton::clicked, this, &CTimerManager::cancel);
  connect (ui->m_add, &QPushButton::clicked, this, &CTimerManager::addClicked);
  connect (ui->m_rem, &QPushButton::clicked, this, &CTimerManager::remClicked);
  connect (ui->m_rep, &QPushButton::clicked, this, &CTimerManager::repClicked);
  connect (ui->m_timers, &QTreeWidget::itemSelectionChanged, this, &CTimerManager::itemSelectionChanged);
  connect (ui->m_timers, &QTreeWidget::itemChanged, this, &CTimerManager::itemChanged);

  updateAddRep ();
  m_seconds = 0;
  m_minutes = 30;
  m_timerID = startTimer (500);
  ui->m_start->setEnabled (false);
}

void CTimerManager::updatedDelay ()
{
  m_minutes = ui->m_m->currentItem ()->text ().toUInt ();
  m_seconds = ui->m_s->currentItem ()->text ().toUInt ();
  updateAddRep ();
}

void CTimerManager::scrollBarValueChanged ()
{
  auto parent = static_cast<QWidget*>(sender ()->parent ());
  while (qobject_cast<QListWidget *>(parent) == nullptr)
  {
    parent = static_cast<QWidget*>(parent->parent ());
  }

  auto             listWidget = static_cast<QListWidget*>(parent);
  int              hGrid      = listWidget->gridSize ().height ();
  QSize            size       = listWidget->viewport ()->size ();
  QPoint           loc (size.width () / 2, (size.height () - hGrid) / 2);
  QListWidgetItem* item = listWidget->itemAt (loc);
  if (item != nullptr)
  {
    CListWidgetItemManager::remBold (listWidget);
    CListWidgetItemManager::setBold (item, true);
    unsigned* row;
    int       modValue;
    if (listWidget == ui->m_m)
    {
      row      = &m_minutes;
      modValue = 100;
    }
    else
    {
      row      = &m_seconds;
      modValue = 60;
    }

    *row = static_cast<unsigned>(listWidget->row (item) % modValue);
  }

  updateAddRep ();
}

void CTimerManager::secondItemClicked (QListWidgetItem* item)
{
  centerTime (item);
}

void CTimerManager::minuteItemClicked (QListWidgetItem* item)
{
  centerTime (item);
}

void CTimerManager::hourItemClicked (QListWidgetItem* item)
{
  centerTime (item);
}

void CTimerManager::itemSelectionChanged ()
{
  QList<QTreeWidgetItem*> selectedItems = ui->m_timers->selectedItems ();
  bool                    enable        = !selectedItems.isEmpty ();
  if (enable)
  {
    QTreeWidgetItem* item  = selectedItems.first ();
    int              index = ui->m_timers->indexOfTopLevelItem (item);
    if (index != -1)
    {
      ui->m_name->setText (m_timers[index]->name ());
      unsigned delay = m_timers[index]->initialTime ();
      m_minutes  = delay / 60000;
      m_seconds  = (delay / 1000) % 60;
      centerCurrentTime (ui->m_m);
      centerCurrentTime (ui->m_s);
    }
  }

  ui->m_rem->setEnabled (enable);
}

void CTimerManager::itemChanged (QTreeWidgetItem *item, int column)
{
  if (column == 0)
  {
    bool enable = item->checkState (column) == Qt::Checked;
    int  iItem   = ui->m_timers->indexOfTopLevelItem (item);
    m_timers[iItem]->setEnabled (enable);
    setStartButton ();
    if (!enable && m_timers[iItem]->isActive ())
    {
      m_timers[iItem]->stop ();
    }
  }
}

void CTimerManager::centerTime (QListWidgetItem* item)
{
  QListWidget* lw = item->listWidget ();
  lw->scrollToItem (item, QAbstractItemView::PositionAtCenter);
  CListWidgetItemManager::remBold (lw);
  CListWidgetItemManager::setBold (item, true);
}

void CTimerManager::centerCurrentTime (QListWidget* listWidget)
{
  int              row  = listWidget == ui->m_m ? 50 * maxRepeats + m_minutes : 30 * maxRepeats + m_seconds;
  QListWidgetItem* item = listWidget->item (row);
  if (item != nullptr)
  {
    listWidget->scrollToItem (item, QAbstractItemView::PositionAtCenter);
    CListWidgetItemManager::remBold (listWidget);
    CListWidgetItemManager::setBold (item, true);
  }
}

void CTimerManager::updateTimers ()
{
  auto formatItem = [] (CTimer const * const timer) -> QStringList
  {
    unsigned ms = timer->initialTime () & (~0x80000000);
    unsigned s  = ms / 1000;
    unsigned m  = s / 60;
    s           = s - (m * 60);

    QLatin1Char const zero ('0');
    QStringList       texts;
    texts << QString ("%1:%2").arg (m, 2, 10, zero).arg (s, 2, 10, zero) << timer->name ();
    return texts;
  };

  auto compare = [] (CTimer const * const t1, CTimer const * const t2) -> bool
  {
    return t1->initialTime () < t2->initialTime ();
  };

  ui->m_timers->clear ();
  std::sort (m_timers.begin (), m_timers.end (), compare);
  for (CTimer* timer : m_timers)
  {
    QTreeWidgetItem* item = new QTreeWidgetItem (ui->m_timers, formatItem (timer));
    item->setCheckState (0, timer->isEnabled () ? Qt::Checked : Qt::Unchecked);
    if (timer->isActive ())
    {
      item->setIcon (0, QIcon (":/icons/sandglass.png"));
      item->setFlags (item->flags () & ~(Qt::ItemIsSelectable/* | Qt::ItemIsUserCheckable*/ | Qt::ItemIsEnabled));
    }
  }
}

bool CTimerManager::exists (unsigned delay) const
{
  bool ex = false;
  for (CTimer const * const timer : m_timers)
  {
    if (timer->initialTime () == delay)
    {
      ex = true;
      break;
    }
  }

  return ex;
}

void CTimerManager::updateAddRep ()
{
  bool none = !exists (delay ());
  ui->m_add->setEnabled (none);
  ui->m_rep->setEnabled (!ui->m_timers->selectedItems ().isEmpty ());
}

bool isEqual (QList<CTimer*> const & t1s, QList<CTimer*> const & t2s)
{
  bool equal = t1s.size () == t2s.size ();
  if (equal)
  {
    int cEqual = 0;
    for (CTimer const * t1 : t1s)
    {
      for (CTimer const * t2 : t2s)
      {
        cEqual += *t1 == *t2 ? 1 : 0;
      }
    }

    equal = cEqual == t1s.size ();
  }

  return equal;
}

bool isNotEqual (QList<CTimer*> const & t1s, QList<CTimer*> const & t2s)
{
  return !isEqual (t1s, t2s);
}

void CTimerManager::setStartButton ()
{
  bool save = false;
  for (CTimer const * timer : m_timers)
  {
    if (timer->isEnabled ())
    {
      save = true;
      break;
    }
  }

  ui->m_start->setText (save ? tr ("Save and start") : tr ("Save"));
  ui->m_start->setEnabled (isNotEqual (m_timers, m_timersBackup));
}

void CTimerManager::timerEvent (QTimerEvent* event)
{
  if (event->timerId () == m_timerID)
  {
    centerCurrentTime (ui->m_m);
    centerCurrentTime (ui->m_s);
    killTimer (m_timerID);
  }
}

void CTimerManager::resizeEvent (QResizeEvent* event)
{
  QDialog::resizeEvent (event);
  QListWidget* lws [] = { ui->m_s, ui->m_m };
  for (QListWidget* lw : lws)
  {
    QScrollBar* sb = lw->verticalScrollBar ();
    sb->blockSignals (true);
    centerCurrentTime (lw);
    sb->blockSignals (false);
  }
}

void CTimerManager::start ()
{
  accept ();
}

void CTimerManager::cancel ()
{
  int          cTimers = m_timers.size ();
  QVector<int> indices;
  indices.reserve (cTimers);
  for (int iTimer = 0; iTimer < cTimers; ++iTimer)
  {
    if (!m_timers[iTimer]->isActive ())
    {
      indices.push_back (iTimer);
    }
  }

  for (int index = indices.size () - 1; index >= 0; --index)
  {
    delete m_timers[indices[index]];
    m_timers.removeAt (indices[index]);
  }

  for (CTimer* timer : m_timersBackup)
  {
    if (timer->initialTime () != 0)
    {
      m_timers.push_back (timer);
    }
    else
    {
      delete timer;
    }
  }

  reject ();
}

void CTimerManager::addClicked ()
{
  auto timer = new CTimer;
  timer->setRemainingTime (delay ());
  timer->setName (ui->m_name->text ());
  m_timers.push_back (timer);
  updateTimers ();
  setStartButton ();
}

void CTimerManager::remClicked ()
{
  QList<QTreeWidgetItem*> selectedItems = ui->m_timers->selectedItems ();
  if (!selectedItems.isEmpty ())
  {
    int index = ui->m_timers->indexOfTopLevelItem (selectedItems.first ());
    if (!m_timers[index]->isActive ())
    {
      delete m_timers[index];
      m_timers.removeAt (index);
      updateTimers ();
    }
  }

  updateAddRep ();
  ui->m_start->setEnabled (isNotEqual (m_timers, m_timersBackup));
  setStartButton ();
}

void CTimerManager::repClicked ()
{
  QList<QTreeWidgetItem*> selectedItems = ui->m_timers->selectedItems ();
  if (!selectedItems.isEmpty ())
  {
    int index = ui->m_timers->indexOfTopLevelItem (selectedItems.first ());
    if (index >= 0)
    {
      CTimer* timer = m_timers[index];
      timer->stop ();
      timer->setRemainingTime (delay ());
      timer->setName (ui->m_name->text ());
      updateTimers ();
    }
  }

  ui->m_start->setEnabled (isNotEqual (m_timers, m_timersBackup));
  setStartButton ();
}
