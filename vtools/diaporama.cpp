#include "diaporama.hpp"
#include <QMutex>
#include <QSize>
#include <QWidget>
#include <QImageReader>

CDiaporama::CDiaporama (int& index, QMutex& mutex, QImage& texture, QStringList const & images,
                        QWidget* widget, EMode mode, float attenuation) : m_index (index),
                         m_mutex (mutex), m_texture (texture), m_images (images), m_widget (widget),
                         m_mode (mode), m_attenuation (attenuation)
{

}

void CDiaporama::run ()
{
  QSize  widgetSize = m_widget->size ();
  QImageReader imageReader;
  imageReader.setFileName (m_images[m_index]);
  imageReader.setAutoTransform (true);

  QImage image    = imageReader.read ();
  image           = image.scaled (widgetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).mirrored ();
  QSize imageSize = image.size ();
  float wh        = widgetSize.height ();
  float ih        = imageSize.height ();
  float iw        = imageSize.width ();
  QRect rc;
  if (wh != ih)
  {
    if (m_mode == FullImage)
    {
      rc = QRect (0, -0.5f * (wh - ih), iw, wh);
    }
    else
    { // CropImage
      int dx = iw * (1.0f - ih / wh);
      rc     = QRect (0.5f * dx, 0, iw - dx, ih);
    }
  }
  else
  {
    float ww = widgetSize.width ();
    if (ww != iw)
    {
      if (m_mode == FullImage)
      {
        rc = QRect (-0.5f * (ww - iw), 0, ww, wh);
      }
      else
      { // CropImage
        int dy = ih * (1.0f - iw / ww);
        rc     = QRect (0, 0.5f * dy, iw, ih - dy);
      }
    }
  }

  if (!rc.isEmpty ())
  {
    image = image.copy (rc);
  }

  image = applyAttenuation (image, m_attenuation);
  ++m_index;
  if (m_index == m_images.size ())
  {
    m_index = 0;
  }

  if (m_mutex.tryLock ())
  {
    m_texture = image;
    m_mutex.unlock ();
  }
}

QImage CDiaporama::applyAttenuation (QImage image, float attenuation)
{
  if (attenuation != 1.0f)
  {
    uchar* bits = image.bits ();
    for (int i = 0, count = image.byteCount (); i < count; i += 4)
    {
      bits[i]     /= attenuation;
      bits[i + 1] /= attenuation;
      bits[i + 2] /= attenuation;
    }
  }

  return image;
}
