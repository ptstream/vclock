#ifndef BRIGHTNESSSENSORINTERFACE_HPP
#define BRIGHTNESSSENSORINTERFACE_HPP

#include "version.hpp"
#include <QtPlugin>

/*! \Brief The sensor brightness plugin. */
class VTOOLSSHARED_EXPORT CBrightnessSensorInterface : public CVersion
{
public:
  /*! Constructor. */
  CBrightnessSensorInterface ();

  /*! Destructor. */
  virtual ~CBrightnessSensorInterface ();

  /*! Sets the sensor identifier. */
  virtual void setId (QByteArray const & id);

  /*! Returns the sensor identifier. */
  virtual QByteArray const & id () const;

  /*! Returns true if the plugin is correctly connected. */
  virtual bool connected () const = 0;

  /*! Returns the brightness. */
  virtual float value () const = 0;

protected :
  QByteArray m_id;
};

#define BRIGHTNESSSENSORINTERFACE_IID "vclock.nixie.CBrightnessSensorInterface"
Q_DECLARE_INTERFACE(CBrightnessSensorInterface, BRIGHTNESSSENSORINTERFACE_IID)

#endif // BRIGHTNESSSENSORINTERFACE_HPP
