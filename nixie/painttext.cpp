#include "nixie.hpp"
#include <QPainter>

void CNixie::paintText ()
{
  QPainter painter (this);
  painter.beginNativePainting ();
  painter.setRenderHints (QPainter::TextAntialiasing, true);
  painter.setRenderHints (QPainter::HighQualityAntialiasing, true);
  painter.setRenderHints (QPainter::Antialiasing, true);

  QFont font = painter.font ();
  QRect rc   = rect ();
  if (m_move)
  {
    rc.setLeft (m_motionTrans.x () * rc.width () * 2);
    rc.setTop (m_motionTrans.y () * rc.height () * 5);
  }

  int cNixieTubes = m_cNixieTubes;
  if (cNixieTubes != 3)
  {
    --cNixieTubes;
  }

  int size = rc.width () / cNixieTubes;
  font.setPixelSize (size);
  if (!m_fontFamily.isEmpty ())
  {
    font.setFamily (m_fontFamily);
  }

  painter.setFont (font);

  float c = 230.0f / m_lightAttenuation;
  QColor primaryColor (m_primaryColor.x () * c, m_primaryColor.y () * c, m_primaryColor.z () * c);
  painter.setPen (primaryColor);
//  painter.setBrush (primaryColor);
  int     d0, d1;
  QString text;
  if (m_cNixieTubes == 6)
  {
    d0   = 2 * rc.width () / 7;
    d1   = d0 / 4;

    rc.setRight (rc.left () + d0);
    text.append (m_digits[0] + '0');
    text.append (m_digits[1] + '0');
    painter.drawText (rc, Qt::TextDontClip | Qt::AlignRight | Qt::AlignVCenter, text);

    rc.setLeft (rc.right ());
    rc.setRight (rc.left () + d1);
    if (m_separator)
    {
      painter.drawText (rc, Qt::TextDontClip | Qt::AlignRight | Qt::AlignVCenter, ":");
    }

    rc.setLeft (rc.left () + d1);
    rc.setRight (rc.left () + d0);
    text.clear ();
    text.append (m_digits[2] + '0');
    text.append (m_digits[3] + '0');
    painter.drawText (rc, Qt::TextDontClip | Qt::AlignCenter, text);

    rc.setLeft (rc.left () + d0);
    rc.setRight (rc.left () + d1);
    if (m_separator)
    {
      painter.drawText (rc, Qt::TextDontClip | Qt::AlignLeft | Qt::AlignVCenter, ":");
    }

    rc.setLeft (rc.left () + d1);
    rc.setRight (rc.left () + d0);
    text.clear ();
    text.append (m_digits[4] + '0');
    text.append (m_digits[5] + '0');
    painter.drawText (rc, Qt::TextDontClip | Qt::AlignLeft | Qt::AlignVCenter, text);
  }
  else if (m_cNixieTubes == 4)
  {
    d0 = 4 * rc.width () / 9;
    d1 = d0 / 4;

    rc.setRight (rc.left () + d0);
    text.append (m_digits[0] + '0');
    text.append (m_digits[1] + '0');
    painter.drawText (rc, Qt::TextDontClip | Qt::AlignRight | Qt::AlignVCenter, text);

    rc.setLeft (rc.right ());
    rc.setRight (rc.left () + d1);
    if (m_separator)
    {
      painter.drawText (rc, Qt::TextDontClip | Qt::AlignCenter, ":");
    }

    rc.setLeft (rc.right ());
    rc.setRight (rc.left () + d0);
    text.clear ();
    text.append (m_digits[2] + '0');
    text.append (m_digits[3] + '0');
    painter.drawText (rc, Qt::TextDontClip | Qt::AlignLeft | Qt::AlignVCenter, text);
  }
  else
  {
    if (m_minus)
    {
      text.append ('-');
    }

    text.append (m_digits[0] + '0');
    text.append (m_digits[1] + '0');
    text.append ('.');
    text.append (m_digits[2] + '0');
    painter.drawText (rc, Qt::TextDontClip | Qt::AlignCenter, text);
  }

  painter.end ();
}
