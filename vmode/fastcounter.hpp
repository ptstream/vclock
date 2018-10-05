#ifndef FASTCOUNTER_HPP
#define FASTCOUNTER_HPP

#include "mode.hpp"

/*! \brief This class is the fast counter manager.
 * The fast counter is just used to measure fps.
 */
class CFastCounter : public CMode
{
public:
  /*! Constructor. */
  CFastCounter ();

  /*! Sets a 0 the fast counter value. */
  void reset () { m_value = 0; }

  /*! Incremenys by 1 the value. */
  void increment () { ++m_value; }

  /*! Returns digits from current value (see CMode::digits). */
  virtual int digits () const;

protected :
  int m_value = 0; //!< Fast counter value.
};

#endif // FASTCOUNTER_HPP
