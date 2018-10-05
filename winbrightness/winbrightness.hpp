#ifndef WINBRIGHTNESS_HPP
#define WINBRIGHTNESS_HPP

#include "winbrightness_global.hpp"
#include "../vtools/brightnessinterface.hpp"

/*! \brief This class manages the backlight windows screen. */
class WINBRIGHTNESSSHARED_EXPORT CWinBrightness  : public QObject, public CBrightnessInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID BRIGHTNESSINTERFACE_IID FILE "winbrightness.json")
  Q_INTERFACES (CBrightnessInterface)
public:
  /*! Contructor. */
  CWinBrightness ();

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
  char const * m_fileName = "\\\\.\\LCD";

private :
  bool m_done = false;
};

#endif // WINBRIGHTNESS_HPP
