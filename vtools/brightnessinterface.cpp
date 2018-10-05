#include "brightnessinterface.hpp"

CBrightnessInterface::CBrightnessInterface ()
{
}

CBrightnessInterface::~CBrightnessInterface ()
{
}

void CBrightnessInterface::setBounds (int min, int max)
{
  m_min = static_cast<float>(min);
  m_max = static_cast<float>(max);
}

int CBrightnessInterface::softwareValue (float y) const
{
  return (y - m_min) / (m_max - m_min);
}

int CBrightnessInterface::hardwareValue (int x) const
{
  return (m_max - m_min) * x + m_min;
}
