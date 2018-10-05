#ifndef ALARM_HPP
#define ALARM_HPP

#include "vtools_globals.h"
#include <QTimer>
#include <QDateTime>

/*!
 * \brief The CAlarm class is the alarm manager.
 */
class VTOOLSSHARED_EXPORT CAlarm : public QTimer
{
  Q_OBJECT
public:
  /*! Contructor. */
  CAlarm ();

  /*! Equality operator. */
  bool operator == (CAlarm const & other) const { return !(*this != other); }

  /*! Inequality operator. */
  bool operator != (CAlarm const & other) const;

  /*! Clone this alarm. */
  CAlarm* clone () const;

  /*! Sets alarm days of week. */
  void setDays (unsigned days) { m_days = days; }

  /*! Adds alarm days of week. */
  void addDays (unsigned day) { m_days |= 1 << (day - 1); }

  /*! Returns alarm days of week. */
  unsigned days () const { return m_days; }

  /*! Sets the date and time of the first day alarm.*/
  void setDateTime (QDateTime const & dateTime) { m_dateTime = dateTime; }

  /*! Returns the date and time of the first day alarm.*/
  QDateTime const & dateTime () const { return m_dateTime; }

  /*! Sets the alarm name.*/
  void setName (QString const & name) { m_name = name; }

  /*! returns the alarm name.*/
  QString const & name () const { return m_name; }

  /*! Sets the widget attached at the alarm. */
  void setWidget (QWidget* widget) { m_widget = widget; }

  /*! Returns the widget attached at the alarm. */
  QWidget* widget () const { return m_widget; }

  /*! Sets the alarm enabled state. */
  void setEnabled (bool enable);

  /*! Returns the enabled state of the alarm. */
  bool isEnabled () const { return (m_days & 0x80000000) == 0; }

  /*! Returns the activated state  of the alarm. */
  bool active () const { return QTimer::isActive () || repeatTimerActive (); }

  /*! Returns the repeat state  of the alarm. */
  bool repeatTimerActive () const { return m_repeatTimer.isActive (); }

  /*! Starts the alarm. */
  void start ();

  /*! Stops the alarm.
   * \param emitedSignal: True the signal stopped is emmited.
   */
  void stop (bool emitSignal = true);

  /*! Returns the enabled state for a day of week. */
  bool isEnabledFor (unsigned day) const { return (m_days & (1 << (day - 1))) != 0; }

  /*! Returns the next day of week. */
  int nextDayHowLong ();

  /*! Returns the remaining number of repeats. */
  unsigned repeatCount () { return m_repeatCount; }

  /*! Converts the alarm data in string. */
  QString toString () const;

  /*! Changes the number of repeats. */
  static void setRepeatCount (unsigned count) { m_initialRepeatCount = count; }

  /*! Returns the number of repeats. */
  static unsigned initialRepeatCount () { return m_initialRepeatCount; }

  /*! Changes the intervals of repeat. */
  static void setRepeatInterval (unsigned interval) { m_repeatInterval = interval; }

  /*! returns the intervals of repeat. */
  static unsigned repeatInterval () { return m_repeatInterval; }

  /*! Changes the format of the date (see QDtaTime). */
  static void setDateFormat (int format) { m_dateFormat = format; }

  /*! Returns the format of the date (see QDtaTime). */
  static int dateFormat () { return m_dateFormat; }

protected slots :
  /*! Alarm timeout. */
  void timeout ();

signals :
  /*! Sent when the alarm stops. */
  void stopped ();

  /*! Sent when the repeat starts. */
  void repeatStarted (unsigned index);

  /*! Sent when the alarm restarts for a next day. */
  void restarted ();

private :
  QString m_name; //!< Alarm name.
  unsigned m_days = 0x7f; //!< Days of week.
  unsigned m_repeatCount; //!< Remaining repeats.
  QDateTime m_dateTime; //!< The date and time of the alarm.
  QTimer m_repeatTimer; //!< Repeat timer.
  QWidget* m_widget = nullptr; //!< The widget attached at the alarm.

  static unsigned m_repeatInterval;  //!< default 10 mn
  static unsigned m_initialRepeatCount; //!< default 3
  static int m_dateFormat; //!< Default long format
};

inline bool operator == (CAlarm const * const alarm, QDateTime const & dt) { return alarm->dateTime () == dt; }

#endif // ALARM_HPP
