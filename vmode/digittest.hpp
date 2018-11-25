#ifndef DIGITTEST_HPP
#define DIGITTEST_HPP

#include "../vmode/mode.hpp"

class CDigitTest : public CMode
{
public:
  CDigitTest ();

  virtual int digits () const;

  bool isCycleFinished () const { return m_count % 20 == 0; }

private :
  mutable int m_value = 0;
  mutable int m_inc   = +1;
  mutable int m_count = 0;
};

#endif // DIGITTEST_HPP
