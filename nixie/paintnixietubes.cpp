#include "nixie.hpp"
#include <QOpenGLTexture>

void CNixie::paintNixieTubes ()
{
  glEnable (GL_CULL_FACE);
  glEnable (GL_DEPTH_TEST);
  glClear (GL_DEPTH_BUFFER_BIT);

  // Draw clock or thermometer
  m_nixieProgram->bind ();

  m_nixieProgram->setUniformValue (m_uVLocs.m_projMatrix, m_proj);
  updateMatrix ();

  QMatrix4x4 m_worldOrg = m_world;

  m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PDigits].m_specular);
  QVector<QVector3D> nixiePos = m_nixiePos[m_cNixieTubes];

  // Positions are percent of bulb diameter.
  float d = m_bulbRadius * 2.0f;
  for (QVector3D& pos : nixiePos)
  {
    pos *= d;
  }

  // Draw enlightened digits.
  CLight light;
  float  step = (2.0f * m_bulbRadius - 3.0f * m_reducingCoefficient) / 11.0f; // 11 intervals for 10 digits;
  float  dz;

  if (isVisiblePart (PCDigits))
  {
    m_nixieProgram->setUniformValue (m_uVLocs.m_digit, true);
    light.setPosition (QVector4D (0.0f, 0.0f, 2.0f * m_bulbHeight, 0));
    light.setIntensities (m_lightings[PDigits].m_light);
    light.apply (m_nixieProgram, 0, CLight::Position | CLight::Intensities);
    m_nixieProgram->setUniformValue (m_uVLocs.m_numLights, 1);

    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PDigits].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PDigits].m_surfaceColor, 1.0f));
    for (int i = 0; i < m_cNixieTubes; ++i)
    {
      EMesh index = static_cast<EMesh>(m_digits[i]);
      dz          = step * (index - 4.5f);
      updateMatrixAndDraw (index, nixiePos[i].x (), nixiePos[i].y (), nixiePos[i].z () + dz);
      m_world = m_worldOrg;
    }

    m_nixieProgram->setUniformValue (m_uVLocs.m_digit, false);
    if (!m_separator)
    {
      light.setIntensities (QVector3D (0.1f, 0.1f, 0.1f));
      light.apply (m_nixieProgram, 0, CLight::Intensities);
    }

    switch (m_cNixieTubes)
    {
      case 3 :
      {
        if (m_minus)
        {
          updateMatrixAndDraw (MMinus, nixiePos[0].x () - m_bulbRadius - removeDim (2.5f),
                             m_bulbHeight / 3.0f,
                             (nixiePos[0].z () + nixiePos[1].z ()) / 2.0f);
          m_world = m_worldOrg;
        }

        updateMatrixAndDraw (MSphere, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                    (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f + 0.8f * m_reducingCoefficient,
                                    (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;
        break;
      }

      case 4 :
      {
        float offset = 0.8f;
        for (int j = 0; j < 2; ++j)
        {
          updateMatrixAndDraw (MSphere, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                            (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f + removeDim (offset),
                            (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
          m_world = m_worldOrg;
          offset += 2.0f;
        }
        break;
      }

      case 6 :
      {
        float offset = 0.8f;
        for (int j = 0; j < 2; ++j)
        {
          for (int i = 1; i <= 3; i += 2 )
          {
            updateMatrixAndDraw (MSphere, (nixiePos[i].x () + nixiePos[i + 1].x ()) / 2.0f,
                                        (nixiePos[i].y () + nixiePos[i + 1].y ()) / 2.0f + removeDim (offset),
                                        (nixiePos[i].z () + nixiePos[i + 1].z ()) / 2.0f);
            m_world = m_worldOrg;
          }

          offset += 2.0f;
        }
        break;
      }
    }
  }

  // Anode + off digits.
  if (isVisiblePart (PCAnode))
  {
    light.setIntensities (m_lightings[PAnode].m_light);
    light.apply (m_nixieProgram, 0, CLight::Intensities);

    m_nixieProgram->setUniformValue (m_uVLocs.m_numLights, 1);
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PAnode].m_surfaceColor, 1.0f));
    float shininess = -1.0f, hLight = -1.0f;
    for (int i = 0; i < m_cNixieTubes; ++i)
    {
      float h = m_bulbHeight * m_anodeLightPositions[m_digits[i]];
      if (h != hLight)
      {
        hLight = h;
        light.setPosition (QVector4D (0, hLight, 0, 0));
        light.apply (m_nixieProgram, 0, CLight::Position);
      }

      if (shininess != m_anodeShininess[m_digits[i]])
      {
        shininess = m_anodeShininess[m_digits[i]];
        m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, shininess);
      }

      updateMatrixAndDraw (MAnode, nixiePos[i].x (), nixiePos[i].y (), nixiePos[i].z ());
      dz = -5.5f * step;
      m_world.translate (0.0f, 0.0f, dz);
      for (int j = MTZero; j <= MTNine; ++j)
      {
        m_world.translate (0, 0, step);
        if (j - 10 != m_digits[i])
        {
          updateMatrixAndDraw (static_cast<EMesh>(j));
        }
      }

      m_world = m_worldOrg;
    }

    switch (m_cNixieTubes)
    {
      case 3 :
      {
        if (!m_minus)
        {
          updateMatrixAndDraw (MMinus, nixiePos[0].x () - m_bulbRadius - removeDim (2.5f),
                             m_bulbHeight / 3.0f,
                             (nixiePos[0].z () + nixiePos[1].z ()) / 2.0f);
          m_world = m_worldOrg;
        }

        updateMatrixAndDraw (MSepWires, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                      (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                      (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;

        updateMatrixAndDraw (MSphere, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                    (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f + removeDim (2.8f),
                                    (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;

        updateMatrixAndDraw (MMinusWires, nixiePos[0].x () - m_bulbRadius - removeDim (2.5f),
                                        removeDim (-6.55f),
                                        (nixiePos[0].z () + nixiePos[1].z ()) / 2.0f);
        m_world = m_worldOrg;
        break;
      }

      case 4 :
      {
        updateMatrixAndDraw (MSepWires, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                      (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                      (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;
        break;
      }

      case 6 :
      {
        for (int i = 1; i <= 3; i += 2 )
        {
          updateMatrixAndDraw (MSepWires, (nixiePos[i].x () + nixiePos[i + 1].x ()) / 2.0f,
                                        (nixiePos[i].y () + nixiePos[i + 1].y ()) / 2.0f,
                                        (nixiePos[i].z () + nixiePos[i + 1].z ()) / 2.0f);
          m_world = m_worldOrg;
        }
        break;
      }
    }
  }

  // Anode fixture
  if (isVisiblePart (PCFixAnode))
  {
    light.setIntensities (m_lightings[PFixAnode].m_light);
    light.apply (m_nixieProgram, 0, CLight::Intensities);
    for (int i = 0; i < m_cNixieTubes; ++i)
    {
      m_world.translate (nixiePos[i].x (), nixiePos[i].y (), nixiePos[i].z ());

      light.setPosition (QVector4D (0, m_bulbHeight, removeDim (m_digits[i] - 5), 0));
      light.apply (m_nixieProgram, 0, CLight::Position);

      m_nixieProgram->setUniformValue(m_uVLocs.m_mvMatrix, m_camera * m_world);
      QMatrix3x3 normalMatrix = m_world.normalMatrix();
      m_nixieProgram->setUniformValue(m_uVLocs.m_normalMatrix, normalMatrix);
      updateMatrix ();

      m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PFixAnode].m_surfaceColor, 1.0f));
      m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PFixAnode].m_shininess);
      m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PFixAnode].m_specular);
      drawArrays (MFixAnode);

      m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PFixDigits].m_shininess);
      m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PFixDigits].m_surfaceColor, 1.0f));
      m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PFixDigits].m_specular);
      drawArrays (MPins);
      drawArrays (MFixDigits);
      m_world = m_worldOrg;
    }

    for (int i = 0; i < m_cNixieTubes; ++i)
    {
      light.setPosition (QVector4D (0, -m_bulbHeight, removeDim (m_digits[i] - 5), 0));
      light.apply (m_nixieProgram, 0, CLight::Position);

      updateMatrixAndDraw (MFixDigits, nixiePos[i].x (), nixiePos[i].y () + 9.8f * m_reducingCoefficient, nixiePos[i].z ());
      m_world = m_worldOrg;
    }
  }

  // Base
  if (isVisiblePart (PCBase))
  {
    light.setPosition (m_moodLightPositions[0]);
    light.setIntensities (m_lightings[PBase].m_light);
    light.setAmbientCoefficient (0.006f);
    light.apply (m_nixieProgram, 0, CLight::Position | CLight::Intensities | CLight::AmbientCoefficient);

    CLight light2 (m_moodLightPositions[1], m_lightings[PBase].m_light, 0.006f);
    light2.apply (m_nixieProgram, 1, CLight::Position | CLight::Intensities | CLight::AmbientCoefficient);
    m_nixieProgram->setUniformValue (m_uVLocs.m_numLights, 2);

    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PBase].m_surfaceColor, 1.0f));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PBase].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PBase].m_specular);

    for (int i = 0; i < m_cNixieTubes; ++i)
    {
      updateMatrixAndDraw (MBase, nixiePos[i].x (), nixiePos[i].y (), nixiePos[i].z ());
      m_world = m_worldOrg;
    }

    switch (m_cNixieTubes)
    {
      case 3 :
      {
        updateMatrixAndDraw (MSepBase, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                     (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                     (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;

        updateMatrixAndDraw (MMinusBase, nixiePos[0].x () - m_bulbRadius - removeDim (2.0f),
                                       (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                       (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;

        updateMatrixAndDraw (MMinusBase, nixiePos[0].x () - m_bulbRadius - removeDim (1.0f),
                                      (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                      (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;
        break;
      }

      case 4 :
        updateMatrixAndDraw (MSepBase, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                     (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                     (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
        m_world = m_worldOrg;
        break;

      case 6 :
        for (int i = 1; i <= 3; i += 2 )
        {
          updateMatrixAndDraw (MSepBase, (nixiePos[i].x () + nixiePos[i + 1].x ()) / 2.0f,
                                       (nixiePos[i].y () + nixiePos[i + 1].y ()) / 2.0f,
                                       (nixiePos[i].z () + nixiePos[i + 1].z ()) / 2.0f);
          m_world = m_worldOrg;
        }
        break;
    }
  }

  CLight light3 (m_moodLightPositions[2], m_lightings[PBase].m_light, 0.006f);
  light3.apply (m_nixieProgram, 2, CLight::Position | CLight::Intensities | CLight::AmbientCoefficient);
  m_nixieProgram->setUniformValue (m_uVLocs.m_numLights, 3);

  // Top
  if (isVisiblePart (PCTop))
  {
    light.setIntensities (m_lightings[PTop].m_light);
    light.apply (m_nixieProgram, 0, CLight::Intensities);

    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PTop].m_surfaceColor, m_lightings[PTop].m_alpha));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PTop].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PTop].m_specular);
    updateMatrixAndDraw (MTop);
  }

  // Case
  d                   = 1.2f * m_bulbRadius;
  QVector2D offsets[] = { QVector2D (-d,  d),
                          QVector2D ( d,  d),
                          QVector2D ( d, -d),
                          QVector2D (-d, -d),
                        };

  if (isVisiblePart (PCCase))
  {
    drawArrays (MCase);
    // Rubber foot
    if (isVisiblePart (PCRubberFoot))
    {
      for (int i = 0; i < 4; ++i)
      {
        int iPos = i == 0 || i == 3 ? 0 : m_cNixieTubes - 1;
        updateMatrixAndDraw (MRubberFoot, nixiePos[iPos].x () + offsets[i].x (),
                                        -11 * m_reducingCoefficient,
                                        nixiePos[iPos].z () + offsets[i].y ());
        m_world = m_worldOrg;
      }
    }
  }

  // Tag
  if (isVisiblePart (PCTag))
  {
    light.setIntensities (m_lightings[PTag].m_light);
    light.apply (m_nixieProgram, 0, CLight::Intensities);
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PTag].m_surfaceColor, 1.0f));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PTag].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PTag].m_specular);
    updateMatrixAndDraw (MTag);
  }

  if (isVisiblePart (PCFixTop))
  {
    d = 1.7f / 1.2f;
    for (QVector2D& offset : offsets)
    {
      offset *= d;
    }

    int index [] = { 0, m_cNixieTubes - 1, m_cNixieTubes - 1, 0 };
    for (int i = 0; i < 4; ++i)
    {
      updateMatrixAndDraw (MFixTop, nixiePos[index[i]].x () + offsets[i].x (),
                                  nixiePos[index[i]].y (),
                                  nixiePos[index[i]].z () + offsets[i].y ());
      m_world = m_worldOrg;
    }
  }

  if (isVisiblePart (PCFixTag))
  {
    float dx   = 7.1f * m_reducingCoefficient;
    float dy   = -6.9f * m_reducingCoefficient;
    float dz   = 7.6f * m_reducingCoefficient;
    offsets[0] = QVector2D (-dx, dy);
    offsets[1] = QVector2D (dx, dy);
    offsets[2] = QVector2D (dx, dy - removeDim (2.2f));
    offsets[3] = QVector2D (-dx, dy - removeDim (2.2f));
    for (int i = 0; i < 4; ++i)
    {
      updateMatrixAndDraw (MFixTag, offsets[i].x (), offsets[i].y (), dz);
      m_world = m_worldOrg;
    }
  }

  // Table. Draw each quadrangle with its texture direction (k).
  if (isVisiblePart (PCTable))
  {
    if (m_textures[TTable] != nullptr)
    {
      m_textures[TTable]->bind ();
      m_world.translate (0.0f, removeDim (-11.5f), 0.0f);
      updateMatrix ();
      //          front, left, rear, right, top, bottom
      int k[] = {     1,    2,    3,     2,   1,      2 };
      for (int i = 0, j = 0; i < 6; ++i, j += 6)
      {
        m_nixieProgram->setUniformValue (m_uVLocs.m_texActivated, k[i]);
        drawArrays (MTable, j, 6);
      }

      m_textures[TTable]->release ();
    }

    m_world = m_worldOrg;
  }

  // About
  if (isVisiblePart (PCAbout))
  {
    m_nixieProgram->setUniformValue (m_uVLocs.m_texActivated, 1);
    if (m_textures[TAbout] != nullptr)
    {
      m_textures[TAbout]->bind ();
      updateMatrixAndDraw (MAboutRect, -6.6f * m_reducingCoefficient,
                           -8.0f * m_reducingCoefficient,
                           -7.3f * m_reducingCoefficient,
                           GL_TRIANGLE_STRIP);
      m_nixieProgram->setUniformValue (m_uVLocs.m_texActivated, 0);
      m_textures[TAbout]->release ();
    }

    m_world = m_worldOrg;
  }

  // Pencil
  if (isVisiblePart (PCPencil))
  {
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PPencil].m_surfaceColor, m_lightings[PPencil].m_alpha));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PPencil].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PPencil].m_specular);
    m_world.rotate (-90.0f, 0.0f, 1.0f, 0.0f);
    // Due to previous rotation x -> z, z -> -x
    float x = m_cNixieTubes == 3 ? 12.0f : 17.0f;
    updateMatrixAndDraw (MPencil, x * m_reducingCoefficient, -10.9f * m_reducingCoefficient, 2.0f * m_reducingCoefficient);

    m_nixieProgram->setUniformValue (m_uVLocs.m_digit, true);
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PPencil2].m_surfaceColor, m_lightings[PPencil2].m_alpha));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PPencil2].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PPencil2].m_surfaceColor, m_lightings[PPencil2].m_alpha));
    drawArrays (MPencil2);
    m_nixieProgram->setUniformValue (m_uVLocs.m_digit, false);

    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PPencil3].m_surfaceColor, m_lightings[PPencil3].m_alpha));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PPencil3].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PPencil3].m_surfaceColor, m_lightings[PPencil3].m_alpha));
    drawArrays (MPencil3);
    m_world = m_worldOrg;
  }

  // Bulb
  if (isVisiblePart (PCBulb) || isVisiblePart (PCSepBulb))
  {
    light.setIntensities (m_lightings[PBulb].m_light);
    light.apply (m_nixieProgram, 0, CLight::Intensities);

    light3.setIntensities (m_lightings[PBase].m_light * 6.0f);
    light3.apply (m_nixieProgram, 2, CLight::Intensities);

    m_nixieProgram->setUniformValue (m_uVLocs.m_numLights, 3);

    m_nixieProgram->setUniformValue (m_uVLocs.m_surfaceColor, QVector4D (m_lightings[PBulb].m_surfaceColor, m_lightings[PBulb].m_alpha));
    m_nixieProgram->setUniformValue (m_uVLocs.m_shininess, m_lightings[PBulb].m_shininess);
    m_nixieProgram->setUniformValue (m_uVLocs.m_specularColor, m_lightings[PBulb].m_specular);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    if (isVisiblePart (PCBulb))
    {
      for (int i = 0; i < m_cNixieTubes; ++i)
      {
        updateMatrixAndDraw (MBulb, nixiePos[i].x (), nixiePos[i].y (), nixiePos[i].z ());
        m_world = m_worldOrg;
      }
    }

    if (isVisiblePart (PCSepBulb))
    {
      switch (m_cNixieTubes)
      {
        case 3 :
        {
          updateMatrixAndDraw (MMinusBulb, nixiePos[0].x () - m_bulbRadius - removeDim (2.5f),
                                         m_bulbHeight / 3.0f,
                                         (nixiePos[0].z () + nixiePos[1].z ()) / 2.0f);
          m_world = m_worldOrg;

          updateMatrixAndDraw (MSepBulb, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                       (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                       (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
          m_world = m_worldOrg;
          break;
        }

        case 4 :
          updateMatrixAndDraw (MSepBulb, (nixiePos[1].x () + nixiePos[2].x ()) / 2.0f,
                                       (nixiePos[1].y () + nixiePos[2].y ()) / 2.0f,
                                       (nixiePos[1].z () + nixiePos[2].z ()) / 2.0f);
          m_world = m_worldOrg;
          break;

        case 6 :
          for (int i = 1; i <= 3; i += 2 )
          {
            updateMatrixAndDraw (MSepBulb, (nixiePos[i].x () + nixiePos[i + 1].x ()) / 2.0f,
                                         (nixiePos[i].y () + nixiePos[i + 1].y ()) / 2.0f,
                                         (nixiePos[i].z () + nixiePos[i + 1].z ()) / 2.0f);
            m_world = m_worldOrg;
          }
          break;
      }
    }

    glDisable (GL_BLEND);
  }

  m_nixieProgram->release ();
}
