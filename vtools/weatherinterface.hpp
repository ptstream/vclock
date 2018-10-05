#ifndef WEATHERINTERFACE_HPP
#define WEATHERINTERFACE_HPP

#include "version.hpp"
#include "forecast.hpp"
#include <QtPlugin>
#include <QPixmap>
#include <QUrl>

class QNetworkAccessManager;

/*! \Brief The weather interface plugin. */
class VTOOLSSHARED_EXPORT CWeatherInterface : public CVersion
{
public:
  /*! Constructor. */
  CWeatherInterface ();

  /*! Destructor. */
  virtual ~CWeatherInterface ();

  /*! Returns the forecast number. */
  virtual int count () const = 0;

  /*! Returns an array of forecasts.
   * \param nam: A global QNetworkAccessManager.
   */
  virtual QVector<CForecast> const & data (QNetworkAccessManager* nam = nullptr) = 0;

  /*! Returns the advertising url of the provider. */
  virtual QUrl advertising () const = 0;

  /*! Returns the logo of the provider. */
  virtual QPixmap logo () const = 0;

  /*! Returns the pixmap for a code. */
  virtual QPixmap pixmap (int code) = 0;

  /*! Sets the town. */
  void setTown (QString const & value);

  /*! Sets the country. */
  void setCountry (QString const & value);

  /*! Returns the town. */
  QString const & town () const;

  /*! Returns the country. */
  QString const & country () const;

  /*! Returns true if the location is defined. */
  bool hasValidLocation () const;

protected :
  QString m_town; //!< The town.
  QString m_country; //!< The country.
  QVector<CForecast> m_forecasts; //!< The last forecasts.
};

#define WEATHERINTERFACE_IID "vclock.nixie.CWeatherInterface"
Q_DECLARE_INTERFACE(CWeatherInterface, WEATHERINTERFACE_IID)

#endif // WEATHERINTERFACE_HPP
