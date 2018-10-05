#include "stopwatch.hpp"

CStopwatch::CStopwatch () : CMode (Stopwatch)
{
  m_tours.reserve (20);
  m_tours.push_back (0);
}

void CStopwatch::startPause ()
{
  if (m_playing)
  {
    m_tours.last () += elapsed ();
    m_interval       = -1;
    m_tour           = false;
  }
  else
  {
    m_interval = 0;
    start ();
    m_tour = true;
  }

  m_playing = !m_playing;
}

void CStopwatch::tourReset ()
{
  if (m_playing)
  {
    m_tours.last () += elapsed ();
    m_tours.push_back (m_tours.last ());
    restart ();
  }
  else
  {
    m_interval = -1;
    m_tours.clear ();
    m_tours.push_back (0);
    m_playing = false;
  }
}

int CStopwatch::digits () const
{
  int value = 0;
  if (!m_tours.isEmpty ())
  {
    int ms = m_tours.last ();
    if (m_playing)
    {
      ms += elapsed ();
    }

    value  = CMode::digits (ms);
  }

  return value;
}

