#ifndef RPIGPIOBRIGHTNESS_HPP
#define RPIGPIOBRIGHTNESS_HPP

#include "rpigpiobrightness_global.hpp"
#include "../vtools/brightnesssensorinterface.hpp"

/*! \brief This class is used to manage the brightness sensor.
 * The hardware is a photoresistor connected a gpio pin through a pull resistor.
 * For tests, a GL5539 photoresitor with a 47KOhms pull resistance are used.
 * The pull resistor is connected at 3.3 volts pin.
 * The GL5539 is connected at the resistor and the pin of gpio.
 * The other GL5539 pin is connected at the electrical ground.
 * The brightness can take values:
 * \li 0 The low brightness.
 * \li 1 The hight brightness.
 */
class RPIGPIOBRIGHTNESSSHARED_EXPORT CRpiGpioBrightness : public QObject, public CBrightnessSensorInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID BRIGHTNESSSENSORINTERFACE_IID FILE "rpigpiobrightness.json")
  Q_INTERFACES (CBrightnessSensorInterface)
public:
  /*! Contrunctor. */
  CRpiGpioBrightness ();

  /*! Destrfuctor. */
  ~CRpiGpioBrightness ();

  /*! Sets the gpio pin number in sens of WitingPi. E.g. 0 is the pin number 17 of the gpio. */
  virtual void setId (QByteArray const & id);

  /*! Returns the value of the pin [0, 1]. */
  virtual float value () const;

  /*! Returns true if the plugin is correctly connected. */
  virtual bool connected () const;

private :
  bool m_connected = false; //!< The folder /sys/class/gpio/gpio[pin] exists.
  QString m_dir = "/sys/class/gpio/"; //!< The Rasbian gpio folder.
};

#endif // RPIGPIOBRIGHTNESS_HPP
