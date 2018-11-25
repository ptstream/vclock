#ifndef MODE_HPP
#define MODE_HPP

class QTime;

/*! \brief This class is the base class of all working mode. */
class CMode
{
public:
  /*! Mode type. */
  enum EType { Clock, //!< Clock 24 or 12 hours, 4 or 6 digits.
               FastCounter, //!< Fast counter (just for fps measurement).
               Stopwatch, //!< Stopwatch with laps times.
               Thermometer, //!< Thermometer degree Celcius or Fahrenheit (3 digits).
               Timer, //!< Timer.
               DigitTest, //!< Digit test (just for fun).
               LastType //!< Internal use.
             };

  /*! Constructor. */
  CMode (EType type);

  /*! Destructor. */
  virtual ~CMode () {}

  /*! Retur the mode trype. */
  EType type () const { return m_type; }

  /*! Returns the digits.
   * The form is 102030 for 6 digits clock and 10h20'30" and 254 for temperature and 25.4°.
   */
  virtual int digits () const { return 0; }

  /*! Returns the interval of measurement. */
  int interval () const { return m_interval; }

  /*! Returns true if mode is external. */
  bool isExternal () const { return m_external; }

  /*! Update interval from type. */
  void updateInterval ();

  /*! Returns digits from QTime.
   * \param time: The time to convert.
   * \param h24: Convertion format 24 or 12h.
   */
  static int digits (QTime const & time, bool h24 = true);

  /*! Gets digits from milliseconds. */
  static int digits (int ms);

  /*! Returns interval from type. */
  static int interval (EType type);

protected :
  EType m_type; //!< The type of mode.
  int m_interval = 1000; //!< The interval of measurement.
  bool m_external = false; //!< External or internal mode.
};

#endif // MODE_HPP
