#ifndef TIMERMANAGER_HPP
#define TIMERMANAGER_HPP

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QTreeWidgetItem;
class QTimerEvent;
class CTimer;

namespace Ui {
  class CTimerManager;
}

/*! \brief The timer manager. */
class CTimerManager : public QDialog
{
public:
  /*! Constructor.
   * \param timers: Timer list to update.
   * \params parent: The widget parent.
   */
  CTimerManager (QList<CTimer*>& timers, QWidget* parent = nullptr);

  /*! Returns the start value in ms. */
  unsigned delay () const { return (m_minutes * 60 + m_seconds) * 1000; }

protected slots :
  /*! Scrolls minutes and seconds list view. */
  void scrollBarValueChanged ();

  /*! Centers second item. */
  void secondItemClicked (QListWidgetItem* item);

  /*! Centers minute item. */
  void minuteItemClicked (QListWidgetItem* item);

  /*! Centers hour item. */
  void hourItemClicked (QListWidgetItem* item);

  /*! List view selection changed. */
  void itemSelectionChanged ();

  /*! List view item changed. */
  void itemChanged (QTreeWidgetItem *item, int column);

  /*! Centers the current time. */
  void centerCurrentTime (QListWidget* listWidget);

  /*! Update and start timers. */
  void start ();

  /*! Cancel slot. */
  void cancel ();

  /*! Adds a new timer. */
  void addClicked ();

  /*! Removes a new timer. */
  void remClicked ();

  /*! Replaces a timer. */
  void repClicked ();

protected :
  /*! Resize event to update list view. */
  virtual void resizeEvent (QResizeEvent* event);

  /*! Timer slot to update widgets at startup. */
  virtual void timerEvent (QTimerEvent* event);

private :
  /*! Centers an item. */
  void centerTime (QListWidgetItem* item);

  /*! Update timer with widget values. */
  void updateTimers ();

  /*! Update the selected timer with widget values. */
  void updateAddRep ();

  /*! Returns true id a timer has the same delay. */
  bool exists (unsigned delay) const;

  /*! Set the start button state. */
  void setStartButton ();

  /*! Updates the replace button. */
  void updatedDelay ();

private :
  Ui::CTimerManager* ui; //!< Ui definition.
  QList<CTimer*>& m_timers; //!< List of timers
  QList<CTimer*> m_timersBackup;//!< Backup list of timers
  int m_timerID = 0; //!< Timer identifier.
  unsigned m_minutes = 0; //!< Current minutes.
  unsigned m_seconds = 0; //!< Current seconds.
};

bool isEqual (QList<CTimer*> const & m_t1s, QList<CTimer*> const & m_t2s);
bool isNotEqual (QList<CTimer*> const & m_t1s, QList<CTimer*> const & m_t2s);

#endif // TIMERMANAGER_HPP
