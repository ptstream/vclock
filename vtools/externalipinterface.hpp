#ifndef EXTERNALIPINTERFACE_HPP
#define EXTERNALIPINTERFACE_HPP

#include "version.hpp"
#include <QtPlugin>
#include <QHostAddress>

class QNetworkAccessManager;

/*! \Brief The external ip interface plugin. */
class VTOOLSSHARED_EXPORT CExternalIpInterface : public CVersion
{
public:
  /*! Constructor. */
  CExternalIpInterface ();

  /*! Destructor. */
  virtual ~CExternalIpInterface ();

  /*! returns the ip addresse of the router for a external device.
   * \param nam: A global QNetworkAccessManager. If it is null, a new is created.
   */
  virtual QHostAddress data (QNetworkAccessManager* nam = nullptr) = 0;
};

#define EXTERNALIPINTERFACE_IID "vclock.nixie.CExternalIpInterface"
Q_DECLARE_INTERFACE(CExternalIpInterface, EXTERNALIPINTERFACE_IID)

#endif // EXTERNALIPINTERFACE_HPP
