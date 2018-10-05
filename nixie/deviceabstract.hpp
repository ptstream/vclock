#ifndef DEVICEABSTRACT_HPP
#define DEVICEABSTRACT_HPP

#include "../vtools/mode.hpp"
#include <QColor>
#include <QVector3D>
#include <QVariant>
#include <QTimer>

class CDeviceAbstract
{
public:
  enum ESystem { SDefault,
                 SPi3
               };

  CDeviceAbstract ();

  CMode::EType type () const { return m_mode->type (); }
  void setType (CMode::EType type, QVariant const & data = QVariant ());
  CMode* mode () { return m_mode; }
  CMode const * mode () const { return m_mode; }

protected :
  CMode* m_mode = nullptr; //!* Current mode (Clock, Thermometer...
  QVector3D m_primaryColor;
  QColor m_bkgndUniformColor = Qt::black; //!* Uniform background color.
  QString m_bkgndTexture; //!* Background texture file.
  ESystem m_system = SDefault; //!* Host system

  QTimer m_redrawTimer; //!< Redraw timer.
  QTimer m_diaporamaTimer;  //!< Background texture timer.
};

#endif // DEVICEABSTRACT_HPP
