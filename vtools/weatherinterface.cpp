#include "weatherinterface.hpp"

 CWeatherInterface::CWeatherInterface ()
{
}

 CWeatherInterface::~CWeatherInterface ()
{
}

void  CWeatherInterface::setTown (QString const & value)
{
  m_town = value;
}

void  CWeatherInterface::setCountry (QString const & value)
{
  m_country = value;
}

QString const &  CWeatherInterface::town () const
{
  return m_town;
}

QString const & CWeatherInterface::country () const
{
  return m_country;
}

bool CWeatherInterface::hasValidLocation () const
{
  return !m_town.isEmpty () && !m_country.isEmpty ();
}
