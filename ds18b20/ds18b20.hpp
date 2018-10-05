#ifndef DS18B20_HPP
#define DS18B20_HPP

#include "ds18b20_global.hpp"
#include "../vtools/tempsensorinterface.hpp"
#include <QObject>

/*! \brief The CDs18b20 class manages the DS15B20 one wire temperature sensor developped by Dallas. */
class DS18B20SHARED_EXPORT CDs18b20 : public QObject, public CTempSensorInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID TEMPSENSORINTERFACE_IID FILE "ds18b20.json")
  Q_INTERFACES (CTempSensorInterface)
public :
  /*! Constructor. */
  CDs18b20 ();

  /*! Returns the temperature in degree Celcius. */
  virtual float value () const;

  /*! Sets the one wire sensor identifier.
   * The identifier form is for example 28-01430d07010c.
   */
  virtual void setId (QByteArray const & id);

private :
  QString m_fileName; //!< Unix filename containing the temperature.
};

#endif // DS18B20_HPP
