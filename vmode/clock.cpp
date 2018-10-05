#include "clock.hpp"
#include <QTime>

CClock::CClock (bool h24) : CMode (Clock), m_h24 (h24)
{
}

int CClock::digits () const
{
  return CMode::digits (QTime::currentTime (), m_h24);
}

