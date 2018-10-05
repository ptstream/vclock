#ifndef XRANDRBRIGHTNESS_HPP
#define XRANDRBRIGHTNESS_HPP

#include "xrandrbrightness_global.hpp"
#include "../vtools/brightnessinterface.hpp"

/*! \brief This class manages the backlight windows screen. */
class XRANDRBRIGHTNESSSHARED_EXPORT CXrandrBrightness  : public QObject, public CBrightnessInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID BRIGHTNESSINTERFACE_IID FILE "xrandrbrightness.json")
  Q_INTERFACES (CBrightnessInterface)
public:
  /*! Contructor. */
  CXrandrBrightness ();

  /*! Returns the backlight brigthness [0, 1] */
  virtual float value () const;

  /*! Sets the backlight brigthness [0, 1] */
  virtual bool setValue (float value);

  /*! Returns true if the official screen is connected. */
  virtual bool connected () const;

private :
  QString m_displayName;
};

#endif // XRANDRBRIGHTNESS_HPP
