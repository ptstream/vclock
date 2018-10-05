#ifndef BRIGHTNESSINTERFACE_HPP
#define BRIGHTNESSINTERFACE_HPP

#include "version.hpp"
#include <QtPlugin>

/*! \Brief The hardware brightness plugin. */
class VTOOLSSHARED_EXPORT CBrightnessInterface : public CVersion
{
public:
  /*! Constructor. */
  CBrightnessInterface ();

  /*! Destructor. */
  virtual ~CBrightnessInterface ();

  /*! Returns the brigthness [0, 1] */
  virtual float value () const = 0;

  /*! Sets the brigthness [0, 1] */
  virtual bool setValue (float value) = 0;

  /*! Returns true if the screen is connected. */
  virtual bool connected () const = 0;

  /*! Set the min max values. */
  void setBounds (int min, int max);

  /*! Returns the hardware value. */
  int softwareValue (float y) const;

  /*! Returns the software value. */
  int hardwareValue (int x) const;

protected :
  float m_min = 0.0f;
  float m_max = 255.0f;
};

#define BRIGHTNESSINTERFACE_IID "vclock.nixie.CBrightnessInterface"
Q_DECLARE_INTERFACE(CBrightnessInterface, BRIGHTNESSINTERFACE_IID)

#endif // BRIGHTNESSINTERFACE_HPP
