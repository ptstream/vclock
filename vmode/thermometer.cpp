#include "thermometer.hpp"
#include "../vtools/tempsensorinterface.hpp"
#include "../vtools/vtools_globals.h"

CThermometer::CThermometer (bool celcius) : CMode (Thermometer), m_celcius (celcius)
{
}

CThermometer::~CThermometer ()
{
}

int CThermometer::digits () const
{
  auto value = static_cast<int>(VTOOLS_INVALID_TEMPERATURE); // Change -9999.9 in -9999 is important.
  if (m_sensor != nullptr)
  {
    float f = m_sensor->value ();
    if (f != VTOOLS_INVALID_TEMPERATURE)
    {
      if (!m_celcius)
      {
        f = toFahrenheit (f);
      }

      auto c1  = static_cast<int>(f / 10);
      auto c2  = static_cast<int>(f) - 10 * c1;
      auto c3  = static_cast<int>(f * 10.0f) % 10;
      value   = c3 * 100 + c2 * 10 + c1;
    }
  }

  return value;
}
