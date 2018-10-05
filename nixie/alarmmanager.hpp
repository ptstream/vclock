#ifndef ALARMMANAGER_HPP
#define ALARMMANAGER_HPP

#include <QDialog>
#include <QDateTime>

class CAlarm;
class QListWidget;
class QListWidgetItem;
class QTreeWidgetItem;

namespace Ui {
  class CAlarmManager;
}

/*! \brief The alarms manager. */
class CAlarmManager : public QDialog
{
  Q_OBJECT
public:
  /*! Constructor.
   * \param alarms: The list of alarms to update.
   * \params parent widget.
   */
  CAlarmManager (QList<CAlarm*>& alarms, QWidget* parent = nullptr);

  /*! Destructor. */
  virtual ~CAlarmManager ();

protected slots :
  /*! Valid and activate the alarms. */
  void on_m_activate_clicked ();

  /*! Cancel the changes. */
  void on_m_cancel_clicked ();

  /*! Adds a new alarms. */
  void on_m_add_clicked ();

  /*! Validate or invalidate the repeat. */
  void on_m_rep_clicked ();

  /*! Removes a new alarms. */
  void on_m_rem_clicked ();

  /*! Show or hide the calendar. */
  void on_m_showCalendar_clicked ();

  /*! Scroll the hours or minutes. */
  void scrollBarValueChanged ();

  /*! Choose hour. */
  void on_m_hours_itemClicked (QListWidgetItem* item);

  /*! Choose minutes. */
  void on_m_minutes_itemClicked (QListWidgetItem* item);

  /*! Alarm list view selection changed. */
  void on_m_alarms_itemSelectionChanged ();

  /*! Alatm item changed. */
  void on_m_alarms_itemChanged (QTreeWidgetItem *item, int column);

  /*! Calendar selection changed. */
  void on_m_calendar_selectionChanged ();

protected :
  /*! Updates the hours and minutes list view. */
  virtual void resizeEvent (QResizeEvent* event);

  /*! Timer to center the hours and minutes list view at startup. */
  virtual void timerEvent (QTimerEvent* event);

private :
  /*! Center the current time. */
  void centerCurrentTime (QListWidget* listWidget);

  /*! Loads the hours and minutes list view. */
  void loadHM (QListWidget* listWidget, int count);

  /*! Updates the alarms. */
  void updateAlarms ();

  /*! Update the current choosen date time. */
  void updatedDateTime ();

  /*! Returns the days of week. */
  QString daysToString (CAlarm const * alarm) const;

  /*! Loads current values. */
  void loadFromItem (QTreeWidgetItem* item);

  /*! Update days of week buttons. */
  void updateDayBtns (const CAlarm* alarm);

  /*! Update alarm from the dialog widget values. */
  void updateAlarm (CAlarm* alarm);

  /*! Returns true if an alarm corresponds at a date time. */
  bool exists (QDateTime const & dt) const;

  /*! Returns true if an alarm is equal at another. */
  bool exists (CAlarm const & alarm) const;

  /*! Update the repeats. */
  void updateAddRep ();

  /*! Update activate and store button state. */
  void setActivateButton ();

private :
  Ui::CAlarmManager* ui; //!< The UI definition.
  QList<CAlarm*>& m_alarms; //!< The alarms list to update.
  QList<CAlarm*> m_alarmsBackup; //!< A backup of alarms to restore in case of cancel.
  int m_h = -1, m_m = -1; //!< Current item index of hours and minutes list view.
  QDateTime m_dateTime; //!< The current date time corresponding at widget values.
  int m_timerID = 0; //!< Timer identifier. Internal use.
};

bool isEqual (QList<CAlarm*> const & m_a1s, QList<CAlarm*> const & m_a2s);
bool isNotEqual (QList<CAlarm*> const & m_a1s, QList<CAlarm*> const & m_a2s);

#endif // ALARMMANAGER_HPP
