#include "digittest.hpp"

CDigitTest::CDigitTest () : CMode (CMode::DigitTest)
{
}

int CDigitTest::digits () const
{
  int value = m_value;
  int coef    = 10;
  for (int i = 0; i < 5; ++i)
  {
    value   += coef * m_value;
    coef    *= 10;
  }

  m_value += m_inc;
  switch (m_value)
  {
    case -1 :
      m_value = 0;
      m_inc   = +1;
      break;

    case 10 :
      m_value = 9;
      m_inc   = -1;
      break;

    default :
      break;
  }

  ++m_count;
  return value;
}
