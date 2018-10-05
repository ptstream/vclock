#include <mode.hpp>
#include <QTime>

int CMode::digits (QTime const & time, bool h24)
{
  int   value       = 0;
  int   coef        = 10000;
  int   hms[]       = { time.second (), time.minute (), time.hour () };
  if (!h24 && hms[2] > 12)
  {
    hms[2] -= 12;
  }

  for (unsigned i = 0; i < sizeof (hms) / sizeof (int); ++i)
  {
    value += (hms[i] / 10 + (hms[i] % 10) * 10) * coef;
    coef  /= 100;
  }

  return value;
}

int CMode::digits (int ms)
{
  int   value       = 0;
  int   coef        = 10000;
  int   hms[]       = { (ms % 1000) / 10, (ms / 1000) % 60, ms / 60000 };
  for (unsigned i = 0; i < sizeof (hms) / sizeof (int); ++i)
  {
    value += (hms[i] / 10 + (hms[i] % 10) * 10) * coef;
    coef  /= 100;
  }

  return value;
}

void CMode::updateInterval ()
{
  m_interval = interval (m_type);
}


int CMode::interval (EType type)
{
  int interval[] = { 1000,  //Clock
                     0,     //FastCounter
                     50,    //Stopwatch
                     5000,  //Thermometer
                     50,    //Timer
                   };
  return interval[type];
}

