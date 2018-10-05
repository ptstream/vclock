#ifndef IP_API_COM_HPP
#define IP_API_COM_HPP

#include "ip_api_com_global.hpp"
#include "../vtools/nam.hpp"
#include "../vtools/iplocationinterface.hpp"

/*! \brief This class is used to get the town and country from external ip address.
 *
 * For example "http://ip-api.com/json/8.8.8.8" that returns
 * something like that
 * \code
 * {"as":"AS15169 Google LLC",
 *  "city":"Mountain View",
 *  "country":"United States",
 *  "countryCode":"US",
 *  "isp":"Google",
 *  "lat":37.4229,
 *  "lon":-122.085,
 *  "org":"Google",
 *  "query":"8.8.8.8",
 *  "region":"CA",
 *  "regionName":"California",
 *  "status":"success",
 *  "timezone":"America/Los_Angeles",
 *  "zip":"94043"}
 * \endcode
*/
class IP_API_COMSHARED_EXPORT CIp_api_com : public QObject, public CIpLocationInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID IPLOCATIONINTERFACE_IID FILE "ip_api_com.json")
  Q_INTERFACES (CIpLocationInterface)
public:
  /*! Contructor. */
  CIp_api_com ();

  /*! Returns the town and country code. */
  virtual QPair<QString, QString> data (QNetworkAccessManager* nam = nullptr);

private :
  CNam m_nam; //!< The network access manager.
};

#endif // API_IPIFY_ORG_HPP
