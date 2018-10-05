#ifndef AIP_IPIFY_ORG_HPP
#define AIP_IPIFY_ORG_HPP

#include "api_ipify_org_global.hpp"
#include "../vtools/nam.hpp"
#include "../vtools/externalipinterface.hpp"

/*! \brief This class is used to get the external ip address.
 *
 * For this it calls "https://api.ipify.org/?format=json" that returns
 * something like that {"ip":"65.112.215.172"}
 */
class API_IPIFY_ORGSHARED_EXPORT CIp_ipify_org : public QObject, public CExternalIpInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID EXTERNALIPINTERFACE_IID FILE "api_ipify_org.json")
  Q_INTERFACES (CExternalIpInterface)
public:
  /*! Contructor. */
  CIp_ipify_org ();

  /*! Returns the ip address of the router. */
  virtual QHostAddress data (QNetworkAccessManager* nam = nullptr);

private :
  CNam m_nam; //!< The network access manager.
};

#endif // AIP_IPIFY_ORG_HPP
