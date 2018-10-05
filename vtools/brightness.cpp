#include "brightness.hpp"
#include "brightnesssensorinterface.hpp"

CBrightness::CBrightness (CBrightnessSensorInterface* plugin, bool enable, QObject* parent) : QTimer (parent),
                m_plugin (plugin), m_enable (enable)
{
  connect (this, &QTimer::timeout, this, &CBrightness::timeout);
}

void CBrightness::setEnabled (bool enable)
{
  if (m_enable != enable)
  {
    m_lastValue = m_currentValue = -1.0f;
    m_enable    = enable;
    if (enable)
    {
      start ();
    }
    else
    {
      stop ();
      emit changed (1.0f);
    }
  }
}

void CBrightness::start (int interval, int count)
{
  if (m_enable && m_plugin != nullptr)
  {
    if (interval != 0)
    {
      setInterval (interval);
    }

    if (count != 0)
    {
      setCount (count);
    }

    QTimer::start (m_interval);
  }
}

void CBrightness::stop ()
{
  m_cTimeout = 0;
  QTimer::stop ();
}

void CBrightness::timeout ()
{
  float value = m_plugin->value ();
  if (value != -1)
  {
    if (value == m_lastValue)
    {
      ++m_cTimeout;
      if (m_cTimeout == m_count)
      {
        if (value != m_currentValue)
        {
          emit changed (value);
        }

        m_currentValue = value;
        m_cTimeout = 0;
      }
    }
    else
    {
      m_lastValue = value;
      m_cTimeout  = 0;
    }
  }
}
