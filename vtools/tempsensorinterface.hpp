#ifndef TEMPSENSORINTERFACE_HPP
#define TEMPSENSORINTERFACE_HPP

#include "version.hpp"
#include <QtPlugin>

/*! \Brief The temperature sensor interface plugin. */
class VTOOLSSHARED_EXPORT CTempSensorInterface : public CVersion
{
public:
  /*! Constructor. */
  CTempSensorInterface ();

  /*! Destructor. */
  virtual ~CTempSensorInterface ();

  /*! Returns the temperature in degree celcius. */
  virtual float value () const = 0;

  /*! Sets the sensor identifier. */
  virtual void setId (QByteArray const & id);

  /*! Returns the sensor identifier. */
  QByteArray const & id () const;

protected :
  QByteArray m_id; //!< The sensor identifier.
};

#define TEMPSENSORINTERFACE_IID "vclock.nixie.CTempSensorInterface"
Q_DECLARE_INTERFACE(CTempSensorInterface, TEMPSENSORINTERFACE_IID)

#endif // TEMPSENSORINTERFACE_HPP
