#include "light.hpp"
#include <QOpenGLShaderProgram>

template <typename T>
static void setLightUniformValue (QOpenGLShaderProgram* program, char const * property, int index, T const & value)
{
  QByteArray uniformName = "lights[" + QByteArray::number (index) + "]." + property;
  program->setUniformValue (uniformName.constData (), value);
}

CLight::CLight () : m_position (QVector4D (0, 0, 0, 1)), m_intensities (QVector3D (1, 1, 1)),
                    m_attenuation (0), m_ambientCoefficient (0), m_coneAngle (25),
                    m_coneDirection (QVector3D (0, 0, 1))
{

}

void CLight::apply (QOpenGLShaderProgram* program, int lightIndex, int components)
{
  for (unsigned component = 1; components != 0; component <<= 1)
  {
    if ((component & components) != 0)
    {
      components ^= component;
      switch (component)
      {
        case Position :
          setLightUniformValue (program, "position", lightIndex, m_position);
          break;

        case Intensities :
          setLightUniformValue (program, "intensities", lightIndex, m_intensities);
          break;

        case Attenuation :
         setLightUniformValue (program, "attenuation", lightIndex, m_attenuation);
         break;

        case AmbientCoefficient :
          setLightUniformValue (program, "ambientCoefficient", lightIndex, m_ambientCoefficient);
          break;

        case ConeAngle :
          setLightUniformValue (program, "coneAngle", lightIndex, m_coneAngle);
          break;

        case ConeDirection :
          setLightUniformValue (program, "coneDirection", lightIndex, m_coneDirection);
          break;
      }
    }
  }
}
