#include "fastcounter.hpp"

CFastCounter::CFastCounter () : CMode (FastCounter)
{
}

int CFastCounter::digits () const
{
  int current = m_value;
  int value   = 0;
  int coef    = 100000;
  for (int i = 0; i < 6; ++i)
  {
    value   += (current % 10) * coef;
    current /= 10;
    coef    /= 10;
  }

  return value;
}
