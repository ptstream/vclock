#ifndef YAHOOWEATHER_HPP
#define YAHOOWEATHER_HPP

#include "yahooweather_global.hpp"
#include "../vtools/weatherinterface.hpp"
#include "../vtools/forecast.hpp"
#include "../vtools/nam.hpp"
#include <QUrl>

class CNam;

/*!
 * \brief The CYahooWeather class manages the Yahoo weather API.
 * This object needs to get the forecasts avalaible on the Yahoo servers.
 */
class YAHOOWEATHERSHARED_EXPORT CYahooWeather : public QObject, public CWeatherInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID WEATHERINTERFACE_IID FILE "yahooweather.json")
  Q_INTERFACES(CWeatherInterface)

public:
  /*! Constructor. */
  CYahooWeather ();

  /*! Desructor. */
  virtual ~CYahooWeather ();

  /*! Returns the number of forecast in day. */
  virtual int count () const;

  /*! Returns the forecast.
   * \param nam : A global QNetworkAccessManager. If it is null, a new is created.
   */
  virtual QVector<CForecast> const & data (QNetworkAccessManager* nam = nullptr);

  /*! Returns the URL of a provider advertising. */
  virtual QUrl advertising () const;

  /*! Returns the URL of a provider logo. */
  virtual QPixmap logo () const;

  /*! Returns the pixmap corresponding at the forecast code.
   * \param code: The forecast code returning by the data function.
   */
  virtual QPixmap pixmap (int code);

private :
  /*! Decodes the json format of the forecasts. */
  void decode (QByteArray const & data);

  /*! Returns the pixmap corresponding at the forecast code.
   * \param code: The forecast code returning by the data function.
   */
  QPixmap pixmapFromCode (int code);

private :
  CNam m_nam; //!< The network access manager.
};

#endif // YAHOOWEATHER_HPP
