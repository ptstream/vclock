#include <mode.hpp>
#include <QTime>

int CMode::digits (QTime const & time, bool h24)
{
  int   value       = 0;
  int   coef        = 10000;
  int   hmss[]      = { time.second (), time.minute (), time.hour () };
  if (!h24 && hmss[2] > 12)
  {
    hmss[2] -= 12;
  }

  for (int hms : hmss)
  {
    value += (hms / 10 + (hms % 10) * 10) * coef;
    coef  /= 100;
  }

  return value;
}

int CMode::digits (int ms)
{
  int   value       = 0;
  int   coef        = 10000;
  int   hmss[]      = { (ms % 1000) / 10, (ms / 1000) % 60, ms / 60000 };
  for (int hms : hmss)
  {
    value += (hms / 10 + (hms % 10) * 10) * coef;
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

