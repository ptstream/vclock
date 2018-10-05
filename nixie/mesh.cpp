#include "nixie.hpp"
#include <QFile>
#include <cstring>

QVector<GLfloat> CNixie::rectangle ()
{
  QVector<GLfloat> mesh (24);
  mesh[0]  = -1; mesh[1]  = -1; mesh[2]  = 0; mesh[3]  = 0; mesh[4]  = 0; mesh[5]  = 1;
  mesh[6]  = -1; mesh[7]  =  1; mesh[8]  = 0; mesh[9]  = 0; mesh[10] = 0; mesh[11] = 1;
  mesh[12] =  1; mesh[13] = -1; mesh[14] = 0; mesh[15] = 0; mesh[16] = 0; mesh[17] = 1;
  mesh[18] =  1; mesh[19] =  1; mesh[20] = 0; mesh[21] = 0; mesh[22] = 0; mesh[23] = 1;
  return mesh;
}

QVector<GLfloat> CNixie::aboutRectangle ()
{
  float            dx = 8.0f * m_reducingCoefficient;
  float            dy = 2.8f * m_reducingCoefficient;
  QVector<GLfloat> mesh (24);
  mesh[0]  = -dx; mesh[1]  = -dy; mesh[2]  = 0; mesh[3]  = 0; mesh[4]  = 0; mesh[5]  = 1;
  mesh[6]  = -dx; mesh[7]  =  dy; mesh[8]  = 0; mesh[9]  = 0; mesh[10] = 0; mesh[11] = 1;
  mesh[12] =  dx; mesh[13] = -dy; mesh[14] = 0; mesh[15] = 0; mesh[16] = 0; mesh[17] = 1;
  mesh[18] =  dx; mesh[19] =  dy; mesh[20] = 0; mesh[21] = 0; mesh[22] = 0; mesh[23] = 1;
  return mesh;
}

QVector<GLfloat> CNixie::readMesh (QString const & fileName)
{
  float            rMin = std::numeric_limits<float>::max ();
  float            rMax = -rMin;
  float            hMin = rMin;
  float            hMax = rMax;
  QVector<GLfloat> data;
  char             header[80];
  quint32          nt = 0;
  QFile            file (fileName + ".nix");
  if (file.open (QIODevice::ReadOnly))
  {
    if (file.read (header, 80) == 80 &&
        file.read (reinterpret_cast<char*>(&nt), sizeof (quint32)) == sizeof (quint32))
    {
      data.resize (nt * 18);
      file.read (reinterpret_cast<char*>(data.data ()), nt * 18 * sizeof (float));
      file.close ();

      if (m_reducingCoefficient == 0)
      {
        float max = std::numeric_limits<float>::max ();
        float mm[] = { max, max, max, -max, -max, -max };
        for (int i = 0, count = data.size () / 6; i < count; i += 6)
        {
          for (int j = 0; j < 3; ++j)
          {
            mm[j]     = std::min (mm[j], data[i + j]);
            mm[j + 3] = std::max (mm[j + 3], data[i + j]);
          }
        }

        rMin = mm[0];
        rMax = mm[3];
        hMin = mm[1];
        hMax = mm[4];
      }
    }
  }
  else
  {
    file.setFileName (fileName + ".stl");
    if (file.open (QIODevice::ReadOnly))
    {
      if (file.read (header, 80) == 80 && file.read (reinterpret_cast<char*>(&nt), sizeof (quint32)) == sizeof (quint32))
      {
        quint16       attribute;
        quint32 const size = 3 * sizeof (float);
        data.resize (nt * 18);
        for (quint32 it = 0; it < nt; ++it)
        {
          quint32 iFloat = it * 18;
          if (file.read (reinterpret_cast<char*>(&data[iFloat + 3]), size) != size)
          {
            break;
          }

          std::memcpy (&data[iFloat + 9],  &data[iFloat + 3], size);
          std::memcpy (&data[iFloat + 15], &data[iFloat + 3], size);
          if (file.read (reinterpret_cast<char*>(&data[iFloat]), size) != size)
          {
            break;
          }

          if (file.read (reinterpret_cast<char*>(&data[iFloat + 6]), size) != size)
          {
            break;
          }

          if (file.read (reinterpret_cast<char*>(&data[iFloat + 12]), size) != size)
          {
            break;
          }

          if (file.read (reinterpret_cast<char*>(&attribute), sizeof (quint16)) != sizeof (quint16))
          {
            break;
          }

          if (m_reducingCoefficient == 0)
          {
            rMax = std::max (data[iFloat],     rMax);
            rMin = std::min (data[iFloat],     rMin);
            hMax = std::max (data[iFloat + 1], hMax);
            hMin = std::min (data[iFloat + 1], hMin);
          }
        }

        file.close ();
      }
    }
  }

  if (m_reducingCoefficient == 0 && !data.isEmpty ())
  {
    auto removeMoodLightsDim = [this] ()
    {
      for (int i = 0; i < 3; ++i)
      {
        for (int j = 0; j < 3; ++j)
        {
          m_moodLightPositions[i][j] = removeDim (m_iniMoodLightPositions[i][j]);
        }
      }
    };

    float dMin              = std::min (rMin, hMin);
    float dMax              = std::max (rMax, hMax);
    m_reducingCoefficient   = 1.5f * (dMax - dMin) * (m_cNixieTubes / 2 + 1);
    m_reducingCoefficient   = 1.0f / m_reducingCoefficient;
    m_bulbRadius            = (rMax - rMin) * m_reducingCoefficient / 2;
    m_bulbHeight            = (hMax - hMin) * m_reducingCoefficient;
    removeMoodLightsDim ();
  }

  int cFloats = data.count ();
  for (int i = 0; i < cFloats; i += 6)
  {
    data[i]     *= m_reducingCoefficient;
    data[i + 1] *= m_reducingCoefficient;
    data[i + 2] *= m_reducingCoefficient;
  }

  return data;
}
