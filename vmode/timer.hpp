#ifndef TIMER_HPP
#define TIMER_HPP

#include "mode.hpp"
#include <QTimer>

/*! brief This class is the timer manager. */
class CTimer : public QTimer, public CMode
{
public:
  /*! Constructor. */
  CTimer ();

  /*! Equality operator. */
  bool operator == (CTimer const & other) const { return !(*this != other); }

  /*! Inequality operator. */
  bool operator != (CTimer const & other) const;

  /*! Returns digits from current value (see CMode::digits). */
  virtual int digits () const;

  /*! Creates a new timer for this. */
  CTimer* clone () const;

  /*! Returns timer names. */
  QString const & name () const { return m_name; }

  /*! Sets the timer bname. */
  void setName (QString const & name) { m_name = name; }

  /*! Returns true if the timer currently playing. */
  bool playing () const { return isActive (); }

  /*! Sets the timer enabled. */
  void setEnabled (bool enable);

  /*! Returns true if the timer is enabled. */
  bool isEnabled () const { return (m_initialTime & 0x80000000) == 0; }

  /*! Changes the remaining time. */
  void setRemainingTime (unsigned msec);

  /*! Returns the initial time. */
  unsigned initialTime () const { return m_initialTime & (~0x80000000); }

  /*! Sets the initial time. */
  void setInitialTime (unsigned time) { m_initialTime = time; }

  /*! Sets the widget attcached at the timer. */
  void setWidget (QWidget* widget) { m_widget = widget; }

  /*! Returns the widget attached at the timer. */
  QWidget* widget () { return m_widget; }

  /*! Returns the widget attached at the timer. */
  QWidget const * widget () const { return m_widget; }

  /*! Toggles timer to play or pause. */
  void swapState ();

  /*! Restarts the timer. */
  void restart ();

private :
  unsigned m_remainingTime = 0; //!< The remaining time.
  unsigned m_initialTime = 0; //!< The initial time.
  QString m_name; //!< The name.
  QWidget* m_widget = nullptr; //!< The attached widget.
  mutable int m_digits = 0; //!* The current value.
};

Q_DECLARE_METATYPE(CTimer*)
Q_DECLARE_OPAQUE_POINTER(CTimer*)

#endif // TIMER_HPP
