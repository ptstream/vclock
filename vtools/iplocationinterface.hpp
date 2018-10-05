#ifndef IPLOCATIONINTERFACE_HPP
#define IPLOCATIONINTERFACE_HPP

#include "version.hpp"
#include <QtPlugin>
#include <QHostAddress>

class QNetworkAccessManager;

/*! \Brief The location interface plugin. */
class VTOOLSSHARED_EXPORT CIpLocationInterface : public CVersion
{
public:
  /*! Constructor. */
  CIpLocationInterface ();

  /*! Destructor. */
  virtual ~CIpLocationInterface ();

  /*! Returns the town and country code.
   * \param nam: A global QNetworkAccessManager. If it is null, a new is created.
   */
  virtual QPair<QString, QString> data (QNetworkAccessManager* nam = nullptr) = 0;

  /*! Sets the external ip address of the router. */
  virtual void setExternalAddress (QHostAddress const & addr);

protected :
  QHostAddress m_addr; //!< The external ip address of the router.
};

#define IPLOCATIONINTERFACE_IID "vclock.nixie.CIpLocationInterface"
Q_DECLARE_INTERFACE(CIpLocationInterface, IPLOCATIONINTERFACE_IID)

#endif // IPLOCATIONINTERFACE_HPP
