#include "nixie.hpp"
#include <QPainter>
#include <cmath>

struct SBrushParams
{
  QColor m_colorHigh;
  QColor m_colorLow;
  QColor m_white;
  QGradient::Type m_gType = QGradient::NoGradient;
};

static void drawSegment (QPainter &painter, QPoint const & pos, int segmentNo, int segLen,
                         SBrushParams const & brushParams)
{
  QPoint   pt    = pos;
  int      width = segLen / 5;
  QPolygon poly;
  poly.reserve (6);

  auto addPoint = [&poly, &pt] (int x, int y)
  {
    QPoint p = pt + QPoint (x, y);
    poly.append (p);
  };

  switch (segmentNo)
  {
    case 0 :
      addPoint (segLen - 1, 0);
      addPoint (segLen - width - 1, width);
      addPoint (width, width);
      addPoint (0, 0);
      break;

    case 1 :
      pt += QPoint (0 , 1);
      addPoint (width, width);
      addPoint (width, segLen - width / 2 - 2);
      addPoint (0, segLen - 2);
      addPoint (0, 0);
      break;

    case 2 :
      pt += QPoint (segLen - 1 , 1);
      addPoint (0, segLen - 2);
      addPoint (-width, segLen - width / 2 - 2);
      addPoint (-width, width);
      addPoint (0, 0);
      break;

    case 3 :
      pt += QPoint(0, segLen);
      addPoint (width, -width / 2);
      addPoint (segLen - width - 1, -width / 2);
      addPoint (segLen - 1,0);
      if ((width & 1) != 0)
      { // adjust for integer division error
        addPoint (segLen - width - 3, width / 2 + 1);
        addPoint (width + 2, width / 2 + 1);
      }
      else
      {
        addPoint (segLen - width - 1, width / 2);
        addPoint (width, width / 2 );
      }

      addPoint (0, 0);
      break;

    case 4 :
      pt += QPoint (0 , segLen + 1);
      addPoint (width, width / 2);
      addPoint (width, segLen - width - 2);
      addPoint (0, segLen - 2);
      addPoint (0, 0);
      break;

    case 5 :
      pt += QPoint(segLen - 1 , segLen + 1);
      addPoint (0 ,segLen - 2);
      addPoint (-width, segLen - width - 2);
      addPoint (-width, width/2);
      addPoint (0, 0);
      break;

    case 6 :
      pt += QPoint (0 , segLen * 2);
      addPoint (width, -width);
      addPoint (segLen - width - 1, -width);
      addPoint (segLen - 1,0);
      addPoint (0, 0);
      break;

    case 7 :
      pt += QPoint (segLen / 2, segLen * 2);
      addPoint (width, 0);
      addPoint (width, -width);
      addPoint (0, -width);
      addPoint (0, 0);
      break;

    case 8 :
      pt += QPoint (segLen / 2 - width / 2 + 1 , segLen / 2 + width);
      addPoint (width, 0);
      addPoint (width, -width);
      addPoint (0, -width);
      addPoint (0, 0);
      break;

    case 9 :
      pt += QPoint (segLen / 2 - width / 2 + 1 , 3 * segLen / 2 + width);
      addPoint (width, 0);
      addPoint (width, -width);
      addPoint (0, -width);
      addPoint (0, 0);
      break;
  }

  painter.setBrush (brushParams.m_colorHigh);
  painter.drawPolygon (poly);
}

static void drawLedSegment (QPainter &painter, QPoint const & pos, int segmentNo, int segLen,
                            SBrushParams const & brushParams)
{
  /*
  auto realAbs = [] (qreal x) -> qreal
  {
    return x < 0.0f ? -x : x;
  };
  */

  QPoint p[2];
  switch (segmentNo)
  {
    case 0 :
      p[0] = pos;
      p[1] = pos + QPoint (segLen, 0);
      break;

    case 1 :
      p[0] = pos;
      p[1] = pos + QPoint (0, segLen);
      break;

    case 2 :
      p[0] = pos + QPoint (segLen, 0);
      p[1] = pos + QPoint (segLen, segLen);
      break;

    case 3 :
      p[0] = pos + QPoint (0, segLen);
      p[1] = pos + QPoint (segLen, segLen);
      break;

    case 4 :
      p[0] = pos + QPoint (0, segLen);
      p[1] = pos + QPoint (0, segLen + segLen);
      break;

    case 5 :
      p[0] = pos + QPoint (segLen, segLen);
      p[1] = pos + QPoint (segLen, segLen + segLen);
      break;

    case 6 :
      p[0] = pos + QPoint (0, segLen + segLen);
      p[1] = pos + QPoint (segLen, segLen + segLen);
      break;

    case 7 :
      p[0] = p[1] = pos + QPoint (segLen / 2, segLen + segLen);
      break;

    case 8 :
      p[0] = p[1] = pos + QPoint (segLen / 2, segLen / 2);
      break;

    case 9 :
      p[0] = p[1] = pos + QPoint (segLen / 2, 3 * segLen / 2);
      break;
  }

  QPointF center = p[0];
  auto drawPie = [&painter, &brushParams, segLen] (QPointF const & center)
  {
    int             width = segLen / 5;
    QPoint          dp (width / 2, width / 2);
    QRadialGradient radialGrad (center, dp.rx ());
    radialGrad.setColorAt (1.0, brushParams.m_colorLow);
    radialGrad.setColorAt (0.5, brushParams.m_colorHigh);
    radialGrad.setColorAt (0.0, brushParams.m_white);
    painter.setBrush (QBrush (radialGrad));
    QRectF rc (center - dp, center + dp);
    painter.drawPie (rc, 0, 5760);
  };

  QPointF ppt = p[1] - p[0];
  if (ppt.isNull ())
  {
    drawPie (center);
  }
  else
  {
    float inc = static_cast<float>(segLen) / 5;
    if (std::fabs (ppt.x ()) > std::fabs (ppt.y ()))
    {
      for (int i = 0; i <= 5; ++i)
      {
        drawPie (center);
        center.setX (center.x () + inc);
      }
    }
    else
    {
      for (int i = 0; i <= 5; ++i)
      {
        drawPie (center);
        center.setY (center.y () + inc);
      }
    }
  }
}

static unsigned segmentBits (unsigned ch)
{
  unsigned const ss[] = { (1 << 0) | (1 << 1) |  (1 << 2) | (1 << 4) | (1 << 5) | (1 << 6),            // 0  0
                          (1 << 2) | (1 << 5),                                                         // 1  1
                          (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 6),                        // 2  2
                          (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6),                        // 3  3
                          (1 << 1) | (1 << 2) | (1 << 3) | (1 << 5),                                   // 4  4
                          (1 << 0) | (1 << 1) | (1 << 3) | ( 1 << 5) | (1 << 6),                       // 5  5
                          (1 << 0) | (1 << 1) | (1 << 3) | ( 1 << 4) | (1 << 5) | (1 << 6),            // 6  6
                          (1 << 0) | (1 << 2) | (1 << 5),                                              // 7  7
                          (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | ( 1 << 4) | (1 << 5) | (1 << 6), // 8  8
                          (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6),             // 9  9
                          (1 << 7),                                                                    // 10 .
                          (1 << 8) | (1 << 9),                                                         // 11 :
                          (1 << 3),                                                                    // 12 -
                       };

  return ch < sizeof (ss) / sizeof (unsigned) ? ss[ch] : 0;
}

static void drawDigit (QPainter&  painter, unsigned value, QPoint const & pos, int segLen,
                       SBrushParams const brushParams[2], bool separator)
{
  unsigned segBits = segmentBits (value);
  if (value <= 9 && brushParams[0].m_gType == QGradient::RadialGradient)
  {
    unsigned bits = (~segBits) & 0x7f;
    unsigned i    = 0;
    while (bits != 0)
    {
      if ((bits & 1) != 0)
      {
        drawLedSegment (painter, pos, i, segLen, brushParams[0]);
      }

      ++i;
      bits >>= 1;
    }
  }

  unsigned bits = segBits;
  unsigned i    = 0;
  while (bits != 0)
  {
    if ((bits & 1) != 0)
    {
      unsigned index = value > 9 && !separator ? 0 : 1;
      if (brushParams[index].m_gType == QGradient::RadialGradient)
      {
        drawLedSegment (painter, pos, i, segLen, brushParams[index]);
      }
      else
      {
        if (index == 1)
        {
          drawSegment (painter, pos, i, segLen, brushParams[index]);
        }
      }
    }

    ++i;
    bits >>= 1;
  }
}

void CNixie::paintLCD ()
{
  QPainter     painter (this);
  SBrushParams brushParams[2];
  bool         leds = m_display == Leds;
  if (leds)
  {
    int alpha = 80;
    brushParams[0].m_colorHigh = QColor (64, 64, 64, alpha);
    brushParams[0].m_colorLow  = QColor (32, 32, 32, alpha);
    brushParams[0].m_white     = QColor (128, 128, 128, alpha);
    brushParams[0].m_gType     = QGradient::RadialGradient;

    alpha = 255;
    brushParams[1].m_colorHigh = QColor (m_primaryColor.x () * 255, m_primaryColor.y () * 255, m_primaryColor.z () * 255, alpha);
    brushParams[1].m_colorLow  = QColor (m_primaryColor.x () * 100, m_primaryColor.y () * 100, m_primaryColor.z () * 100, alpha);
    brushParams[1].m_white     = QColor (255, 255, 255, alpha);
    brushParams[1].m_gType     = QGradient::RadialGradient;
  }
  else
  {
    /*
    int const alpha = 80;
    brushParams[0].m_colorHigh = QColor (64, 64, 64, alpha);
    brushParams[0].m_colorLow  = QColor (32, 32, 32, alpha);
    brushParams[0].m_white     = QColor (128, 128, 128, alpha);
    brushParams[0].m_gType     = QGradient::NoGradient;
    */

    float c                    = 230.0f / m_lightAttenuation;
    brushParams[1].m_colorHigh = QColor (m_primaryColor.x () * c, m_primaryColor.y () * c, m_primaryColor.z () * c);
    /*
    brushParams[1].m_colorLow  = QColor (brushParams[1].m_colorHigh.red () / 4, brushParams[1].m_colorHigh.green () / 4, brushParams[1].m_colorHigh.blue () / 4, alpha);
    brushParams[1].m_white     = QColor (255, 255, 255, alpha);*/
    brushParams[1].m_gType     = QGradient::NoGradient;
  }

  painter.setPen (Qt::NoPen);
  painter.beginNativePainting ();
  painter.setRenderHints (QPainter::HighQualityAntialiasing, true);
  painter.setRenderHints (QPainter::Antialiasing, true);

  quint8 digits[8];
  int    n;
  switch (m_cNixieTubes)
  {
    case 6 :
      digits[0] = m_digits[0];
      digits[1] = m_digits[1];
      digits[2] = 11;
      digits[3] = m_digits[2];
      digits[4] = m_digits[3];
      digits[5] = 11;
      digits[6] = m_digits[4];
      digits[7] = m_digits[5];
      n         = 8;
      break;

    case 4 :
      digits[0] = m_digits[0];
      digits[1] = m_digits[1];
      digits[2] = 11;
      digits[3] = m_digits[2];
      digits[4] = m_digits[3];
      n         = 5;
      break;

    default :
    {
      int k = 0;
      if (m_minus)
      {
        digits[k++] = 12;
      }

      digits[k++] = m_digits[0];
      digits[k++] = m_digits[1];
      digits[k++] = 10;
      digits[k++] = m_digits[2];
      n           = k;
      break;
    }
  }

  int s  = 2; // Space between digits.
  int w  = this->width (); // Widget width.
  int h  = this->height (); // Widget height.
  int xl = s * w / ((s + 1) * n - 1 + 2 * s); // Computed from w = n * xl + (n - 1) * xl / s + 2 * s;
  int yl = h / 4;
  xl     = yl > xl ? xl : yl; // In case of small h.
  int dx = xl +  xl / s;
  int xo = (w - (n - 1) * dx - xl) / 2;
  int yo = (h - xl * 2) / 2;

  QPoint pos (xo, yo);
  QPoint tr (m_motionTrans.x () * xl * 11, m_motionTrans.y () * yl * 12);
  for (int i = 0; i < n; ++i)
  {
    if (m_move)
    {
      pos += tr;
    }

    drawDigit (painter, digits[i], pos, xl, brushParams, m_separator);
    pos.setX (pos.x () + dx);
  }

  painter.end ();
}
