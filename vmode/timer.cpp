#include "timer.hpp"
#include <QTime>
#include <QDebug>

CTimer::CTimer () : CMode (Timer)
{
  m_external = true;
  setSingleShot (true);
}

void CTimer::swapState ()
{
  if (isActive ())
  {
    m_remainingTime = remainingTime ();
    stop ();
  }
  else
  {
    setInterval (m_remainingTime);
    start ();
  }
}

int CTimer::digits () const
{
  if (isActive ())
  {
    int remainingTime = QTimer::remainingTime ();
    if (remainingTime < 100)
    {
      remainingTime = 0;
    }

    m_digits = CMode::digits (remainingTime);
  }

  return m_digits;
}

CTimer* CTimer::clone () const
{
  auto timer             = new CTimer;
  timer->m_remainingTime = m_remainingTime;
  timer->m_initialTime   = m_initialTime;
  timer->m_name          = m_name;
  timer->m_digits        = m_digits;
  if (isActive ())
  {
    timer->m_remainingTime = timer->m_initialTime = 0;
  }

  return timer;
}

bool CTimer::operator != (CTimer const & other) const
{
  return m_initialTime != other.m_initialTime || m_name != other.m_name;
}

void CTimer::setRemainingTime (unsigned msec)
{
  m_remainingTime = m_initialTime = msec;
  setInterval (msec);
}

void CTimer::restart ()
{
  m_remainingTime = m_initialTime;
  setInterval (m_remainingTime);
  start ();
}

void CTimer::setEnabled (bool enable)
{
  if (enable)
  {
    m_initialTime &= ~0x80000000;
  }
  else
  {
    m_initialTime |= 0x80000000;
  }
}

