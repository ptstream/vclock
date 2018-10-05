#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vtools_globals.h"
#include<QVector4D>
#include<QVector3D>

class  QOpenGLShaderProgram;

/*! \brief The light definition.
 * The light components are:
 * \li The position.
 * \li The intensities.
    \li Attenuation.
    \li Ambient coefficient.
    \li Cone angle.
    \li Cone direction
 */
class VTOOLSSHARED_EXPORT CLight
{
public:
  /*! The light components. */
  enum EComponent { Position = 0x0001, //!< The position.
                    Intensities = 0x0002, //!< The intensities.
                    Attenuation = 0x0004, //!< The attenuation.
                    AmbientCoefficient = 0x0008, //!< Position.
                    ConeAngle = 0x0010, //!< The cone angle in case of spot light.
                    ConeDirection = 0x0020, //!< The cone direction in case of spot light.
                    LastComponent = 0x0040, //!< Internal use.
                  };

  /*! Constructor. */
  CLight ();

  /*! Constructor. */
  inline CLight (QVector4D const & position, QVector3D const & intensities, float ambientCoefficient = 0);

  /*! Constructor. */
  inline CLight (QVector4D const & position, QVector3D const & intensities, float coneAngle,
                 QVector3D const & coneDirection, float attenation = 0, float ambientCoefficient = 0);

  /*! Sets the position. */
  void setPosition (QVector4D const & position) { m_position = position; }

  /*! Sets the intensities. */
  void setIntensities (QVector3D const & intensities) { m_intensities = intensities; }

  /*! Sets the ambient coefficient. */
  void setAmbientCoefficient (float ambientCoefficient) { m_ambientCoefficient = ambientCoefficient; }

  /*! Sets the attenuation. */
  void setAttenuation (float attenuation) { m_attenuation = attenuation; }

  /*! Sets the cone angle. */
  void setConeAngle (float angle) { m_coneAngle = angle; }

  /*! Sets the cone direction. */
  void setConeDirection (QVector3D dir) { m_coneDirection = dir; }

  /*! Apply the light to a shader program.
   * \param program: The shader program.
   * \param lightIndex: The light index.
   * \param Flags to transmit. Or operator can be used for multiple values.
   */
  void apply (QOpenGLShaderProgram* program, int lightIndex, int components);

protected :
  QVector4D m_position; //!<Homogenious coordinates of the light position
  QVector3D m_intensities; //!< The color of the light
  float m_attenuation; //!< Light attenuation
  float m_ambientCoefficient; //!< Ambient coefficient
  float m_coneAngle; //!< Cone angle of light
  QVector3D m_coneDirection; //!< Cone direction of light
};

CLight::CLight (QVector4D const & position, QVector3D const & intensities, float ambientCoefficient) :
           m_position (position), m_intensities (intensities), m_attenuation (0),
           m_ambientCoefficient (ambientCoefficient), m_coneAngle (0)
{
}

CLight::CLight (QVector4D const & position, QVector3D const & intensities, float coneAngle,
                QVector3D const & coneDirection, float attenation, float ambientCoefficient) :
                m_position (position), m_intensities (intensities), m_attenuation (attenation),
                m_ambientCoefficient (ambientCoefficient), m_coneAngle (coneAngle),
                m_coneDirection (coneDirection)
{
}

#endif // LIGHT_HPP
