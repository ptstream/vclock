#include "alarm.hpp"
#include <QDate>
#include <QDebug>

int const oneDayMSecs = 86400000; // 24 hours in ms;

unsigned CAlarm::m_repeatInterval = 600000;
unsigned CAlarm::m_initialRepeatCount = 3;
int CAlarm::m_dateFormat = 0;

CAlarm::CAlarm () : QTimer (), m_repeatCount (m_initialRepeatCount)
{
  setSingleShot (true);
  m_repeatTimer.setInterval (m_repeatInterval);
  connect (this, &QTimer::timeout, this, &CAlarm::timeout);
  connect (&m_repeatTimer, &QTimer::timeout, this, &CAlarm::timeout);
}

void CAlarm::start ()
{
  int ms = QDateTime::currentDateTime ().msecsTo (m_dateTime);
  if (ms <= 0)
  {
    stop ();
  }
  else
  {
    QTimer::start (ms);
  }
}

void CAlarm::stop (bool emitSignal)
{
  QTimer::stop ();
  if (emitSignal)
  {
    emit stopped ();
  }
}

void CAlarm::timeout ()
{
  if (sender () == this)
  {
    if (m_days != 0)
    {
      int cDay = nextDayHowLong ();
      m_dateTime = m_dateTime.addDays (cDay);
      QTimer::start (oneDayMSecs * cDay);
      if (m_repeatCount == 0)
      {
        emit restarted ();
      }
    }

    if (m_repeatCount != 0)
    {
      m_repeatTimer.start ();
      emit repeatStarted (m_repeatCount);
    }
    else
    {
      if (m_days == 0)
      {
        setEnabled (false);
        emit stopped ();
      }
    }
  }
  else
  {
    --m_repeatCount;
    if (m_repeatCount == 0)
    {
      m_repeatCount = m_initialRepeatCount;
      m_repeatTimer.stop ();
      if (m_days == 0)
      {
        setEnabled (false);
        emit stopped ();
      }
      else
      {
        emit restarted ();
      }
    }
    else
    {
      emit repeatStarted (m_repeatCount);
    }
  }
}

bool CAlarm::operator != (CAlarm const & other) const
{
  return m_dateTime    != other.m_dateTime ||
         m_name        != other.m_name     ||
         m_days        != other.m_days     ||
         m_repeatCount != other.m_repeatCount;
}

CAlarm* CAlarm::clone () const
{
  CAlarm* alarm        = new CAlarm;
  alarm->m_dateTime    = m_dateTime;
  alarm->m_name        = m_name;
  alarm->m_days        = m_days;
  alarm->m_repeatCount = m_repeatCount;
  return alarm;
}

void CAlarm::setEnabled (bool enable)
{
  if (enable)
  {
    m_days &= ~0x80000000;
  }
  else
  {
    m_days |= 0x80000000;
  }
}

int CAlarm::nextDayHowLong ()
{
  int day  = QDate::currentDate ().day () + 1;
  int cDay = 0;
  for (int k = day; k < day + 7; ++k)
  {
    int nextDay = k % 7;
    if ((m_days & (nextDay - 1)) != 0)
    {
      break;
    }

    ++cDay;
  }

  return cDay;
}

QString CAlarm::toString () const
{
  QLocale locale = QLocale::system ();
  QDate   date   = QDate::currentDate ();
  int     day    = date.dayOfWeek ();
  QString text   = QString ("%1").arg (locale.toString (m_dateTime, static_cast<QLocale::FormatType>(m_dateFormat)));
  for (int k = 1; k < 8; ++k)
  {
    if (isEnabledFor (k))
    {
      text += ' ' + date.addDays (k - day).toString ("ddd");
    }
  }

  return text;
}
