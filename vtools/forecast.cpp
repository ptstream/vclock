#include "forecast.hpp"
#include "vtools_globals.h"

struct SForecastData : public QSharedData
{
  SForecastData () {}
  SForecastData (SForecastData const & other);

  float m_temp    = VTOOLS_INVALID_TEMPERATURE;
  float m_tempMin = VTOOLS_INVALID_TEMPERATURE;
  float m_tempMax = VTOOLS_INVALID_TEMPERATURE;
  int   m_code    = -1;
  QString m_text;
};

SForecastData::SForecastData (SForecastData const & other) : QSharedData (other),
                    m_temp (other.m_temp),
                    m_tempMin (other.m_tempMin),
                    m_tempMax (other.m_tempMax),
                    m_code (other.m_code),
                    m_text (other.m_text)
{
}

CForecast::CForecast () : m_d (new SForecastData)
{
}

CForecast::CForecast (CForecast const & rhs) : m_d (rhs.m_d)
{
}

CForecast& CForecast::operator = (CForecast const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CForecast::~CForecast()
{
}

float CForecast::temp () const
{
  return m_d->m_temp;
}

float CForecast::tempMin () const
{
  return m_d->m_tempMin;
}

float CForecast::tempMax () const
{
  return m_d->m_tempMax;
}

int CForecast::code () const
{
  return m_d->m_code;
}

QString const & CForecast::text () const
{
  return m_d->m_text;
}

void CForecast::setTemp (float value)
{
  m_d->m_temp = value;
}

void CForecast::setTempMin (float value)
{
  m_d->m_tempMin = value;
}

void CForecast::setTempMax (float value)
{
  m_d->m_tempMax = value;
}

void CForecast::setCode (int value)
{
  m_d->m_code = value;
}

void CForecast::setText (QString const & value)
{
  m_d->m_text = value;
}
