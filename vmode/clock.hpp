#ifndef CLOCK_HPP
#define CLOCK_HPP

#include "mode.hpp"

/*! \brief This class is the clock manager. */
class CClock : public CMode
{
public:
  /*! Constructor.
   * \param h24: 12 or hour display.
   */
  CClock (bool h24);

  /*! Returns 12 or 24 hour display. */
  bool h24 () const { return m_h24; }

  /*! Set the 12 or 24 hour display. */
  void setH24 (bool h24) { m_h24 = h24; }

  /*! Returns digits from current time (see CMode::digits). */
  virtual int digits () const;

private :
  bool m_h24 = true; //!< 12 or 24 hour display.
};

#endif // CLOCK_HPP
