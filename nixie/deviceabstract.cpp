#include "deviceabstract.hpp"

CDeviceAbstract::CDeviceAbstract ()
{
}

void CDeviceAbstract::setType (CMode::EType type, QVariant const & data)
{
  if (m_mode != nullptr)
  {
    CMode::EType currentType = m_mode->type ();
    if (currentType != type)
    {
      if (!m_mode->isExternal ())
      {
        delete m_mode;
      }

      m_mode = nullptr;
    }
    else if (m_mode->isExternal () && data.value<CTimer*> () != m_mode)
    {
      m_mode = nullptr;
    }
  }

  if (m_mode == nullptr)
  {
    m_separator = true;
    switch (type)
    {
      case CMode::Stopwatch :
        m_redrawTimer.stop ();
        m_mode = new CStopwatch ();
        break;

      case CMode::Clock :
        m_redrawTimer.start ();
        m_mode = new CClock (data.toBool ());
        break;

      case CMode::FastCounter :
        m_mode = new CFastCounter ();
        m_redrawTimer.start ();
        break;

      case CMode::Thermometer :
        m_mode = new CThermometer (data.toBool ());
        m_redrawTimer.start ();
        break;

      case CMode::Timer :
        m_redrawTimer.start ();
        m_mode = data.value<CTimer*> ();
        break;

      default :
        break;
    }
  }

  if (m_mode != nullptr)
  {
    int interval = m_mode->interval ();
    m_redrawTimer.setInterval (interval);
    if (type != CMode::Thermometer)
    {
      updateTubes ();
    }
  }
}
