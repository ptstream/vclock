#ifndef RPIBLBRIGHTNESS_HPP
#define RPIBLBRIGHTNESS_HPP

#include "rpiblbrightness_global.hpp"
#include "../vtools/brightnessinterface.hpp"

/*! \brief This class manages the backlight official 7inch screen. */
class RPIBLBRIGHTNESSSHARED_EXPORT CRpiBlBrightness  : public QObject, public CBrightnessInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID BRIGHTNESSINTERFACE_IID FILE "rpiblbrightness.json")
  Q_INTERFACES (CBrightnessInterface)
public:
  /*! Contructor. */
  CRpiBlBrightness ();

  /*! Returns the backlight brigthness [0, 1] */
  virtual float value () const;

  /*! Sets the backlight brigthness [0, 1] */
  virtual bool setValue (float value);

  /*! Returns true if the official screen is connected. */
  virtual bool connected () const;

private :
  QByteArray read () const; //!< Read the brightness value.
  bool write (int value); //!< Write the desired brightness value.

private :
  /*! The Raspberry PI 3 backlight brightness screen file. */
  QString m_fileName = "/sys/class/backlight/rpi_backlight/brightness";
};

#endif // RPIBLBRIGHTNESS_HPP
