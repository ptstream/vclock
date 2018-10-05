#ifndef BRIGHTNESS_HPP
#define BRIGHTNESS_HPP

#include "vtools_globals.h"
#include <QTimer>

class CBrightnessSensorInterface;

/*!
 * \brief The Brightness class is used to emit a signal when the state of a gpio pin change.
 * In the context of vtools, it used to manage the brightness of Raspberry PI screen.
 * For this a photoresistor (e.g. GL5539) is connect at the pin pinIndex throw a pull resistor (e.g. 47kOhms).
 * When the pin voltage change the state of pinIndex change.
 * To limit the state oscillation, the signal is emited after a delay during the pin is at the same state.
 */
class VTOOLSSHARED_EXPORT CBrightness : public QTimer
{
  Q_OBJECT
public:
  /*! Constructor.
   * \param plugin: The brightness sensor plugin.
   * \param pinIndex: The pin index of the gpio.
   * \param parent: The parent QObject.
   */
  CBrightness (CBrightnessSensorInterface* plugin, bool enable = true, QObject* parent = nullptr);

  /*! Set the initial pin state. */
  void setInitialValue (int value) { m_lastValue = m_currentValue = value; }

  /*! Sets the timer interval to confirm the state. By default 5 seconds. */
  void setInterval (int interval) { m_interval = interval; }

  /*! Sets the number of times to confirm the state. By default 6 times. */
  void setCount (int count) { m_count = count; }

  /*! Starts the confimation timer. */
  void start (int interval = 0, int count = 0);

  /*! Stops the confimation timer. */
  void stop ();

  /*! Enable of disable the brightness. */
  void setEnabled (bool enable);

  /*! Returns true if the brightness is enabled. */
  bool isEnabled () const { return m_enable; }

protected slots :
  /*! Timet timeout. */
  void timeout ();

signals :
  /*! The brightness has changed. */
  void changed (float value);

private :
  CBrightnessSensorInterface* m_plugin = nullptr; //!< The brightness sensor plugin.
  int m_count = 6; //!< Number of times use for confirmation.
  int m_interval = 5000; //!< Interval during each confirmation. */
  int m_cTimeout = 0; //!< Current number of timeout.
  float m_lastValue = -1; //!< Last value.
  float m_currentValue = -1; //!< Actual value.
  bool m_enable = true; //!< Enable or disable.
};

#endif // BRIGHTNESS_HPP
