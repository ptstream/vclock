#ifndef FORECAST_HPP
#define FORECAST_HPP

#include "vtools_globals.h"
#include <QSharedDataPointer>

struct SForecastData;

/*! \brief The forecast container. */
class VTOOLSSHARED_EXPORT CForecast
{
public:
  /*! Constructor. */
  CForecast ();

  /*! Copy constructor. */
  CForecast (CForecast const &);

  /*! Assign operator. */
  CForecast& operator = (CForecast const &);

  /*! Destructor. */
  ~CForecast ();

  /*! Returns the actual temperature. */
  float temp () const;

  /*! Returns the minimum temperature. */
  float tempMin () const;

  /*! Returns the maximum temperature. */
  float tempMax () const;

  /*! Returns the actual code associated at the image. */
  int code () const;

  /*! Returns the actual text. */
  QString const & text () const;

  /*! Sets the actual temperature. */
  void setTemp (float value);

  /*! Sets the minimum temperature. */
  void setTempMin (float value);

  /*! Sets the maximum temperature. */
  void setTempMax (float value);

  /*! Sets the actual code associated at the image. */
  void setCode (int value);

  /*! Sets the actual text. */
  void setText (QString const & value);

private:
  QSharedDataPointer<SForecastData> m_d; //!< The forecast data.
};

#endif // FORECAST_HPP
