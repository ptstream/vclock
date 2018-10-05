#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include "mode.hpp"
#include <QTime>
#include <QVector>

/*! \brief This class is the stopwatch manager.
 * Unlimited lap times can be enabled.
 */
class CStopwatch : public QTime, public CMode
{
public:
  /*! Constructor. */
  explicit CStopwatch ();

  /*! Returns digits from current time (see CMode::digits). */
  virtual int digits () const;

  /*! Returns true if stopwatch is in playing mode. */
  bool playing () const { return m_playing; }

  /*! Returns true if lap time is in enable. */
  bool tour () const { return m_tour; }

  /*! Returns the lap times in ms. */
  QVector<int> const & tours () const { return m_tours; }

  /*! Toggles the stopwatch in mode start or pause. */
  void startPause ();

  /*! Reset lap time. */
  void tourReset ();

private :
  bool m_playing = false; //!< Play or pause mode.
  bool m_tour = false; //!< Lap time enable or disable.
  QVector<int> m_tours; //!< Lap time values in ms
};

#endif // STOPWATCH_HPP
