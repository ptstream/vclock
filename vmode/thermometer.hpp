#ifndef THERMOMETER_HPP
#define THERMOMETER_HPP

#include "mode.hpp"

class CTempSensorInterface;

/*! \brief This class is the thermometer manager.
 * The temperature can be in degree Celcius or Fahrenheit
 */
class CThermometer : public CMode
{
public:
  /*! Constructor. */
  CThermometer (bool celcius);

  /*! Destructor. */
  ~CThermometer ();

  /*! Returns true if the display is in degree Celcius. */
  bool celcius () const { return m_celcius; }

  /*! Sets the units.
   * \param celcius: True for degree Celcius, false for degree  * The temperature can be in degree Celcius or Fahrenheit.
   */
  void setCelcius (bool celcius) { m_celcius = celcius; }

  /*! Sets the sensor plugin. */
  void setSensor (CTempSensorInterface* sensor) { m_sensor = sensor; }

  /*! Returns the sensor plugin. */
  CTempSensorInterface* sensor () { return m_sensor; }

  /*! Returns the sensor plugin. */
  CTempSensorInterface const * sensor () const { return m_sensor; }

  /*! Degree Celcius conversion. */
  static float toCelcius (float t) { return (t - 32.0f) * 5.0f / 9.0f ; }

  /*! Degree Fahrenheit conversion. */
  static float toFahrenheit (float t) { return 9.0f * t / 5.0f + 32.0f; }

  /*! Returns digits from current temperature. */
  virtual int digits () const;

private :
  bool m_celcius = true; //!< Celcius or Fahrenheit
  CTempSensorInterface* m_sensor = nullptr; //!< The sensor plugin.
};

#endif // THERMOMETER_HPP
