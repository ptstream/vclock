#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include "vtools_globals.h"
#include <QVector3D>

/*! \brief OpenGl lighting definition. */
struct VTOOLSSHARED_EXPORT CLighting
{
  /*! Constructor. */
  CLighting ();

  /*! Copy constructor. */
  CLighting (CLighting const & other) { *this = other; }

  /*! Equal operator. */
  CLighting& operator = (CLighting const & other)
  {
    m_light        = other.m_light;
    m_specular     = other.m_specular;
    m_surfaceColor = other.m_surfaceColor;
    m_shininess    = other.m_shininess;
    m_alpha        = other.m_alpha;
    return *this;
  }

public :
  QVector3D m_light; //!< Light color.
  QVector3D m_specular; //!< Specular color component.
  QVector3D m_surfaceColor; //!< Surface color component.
  float m_shininess; //!< Surface shininess.
  float m_alpha = 1.0f; //!< Transparency.
};

#endif // LIGHTING_HPP
