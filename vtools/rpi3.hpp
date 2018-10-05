#ifndef RPI3_HPP
#define RPI3_HPP

#include "vtools_globals.h"
#include <QString>
#include <QIODevice>

/*! \brief A simple Raspberry PI 3 manager.
 * Actually only blPower and brightness of the official 7" screen is implemented.
 */
class VTOOLSSHARED_EXPORT CRpi3
{
public:
  /*! Constructor. */
  CRpi3 ();

  /*! Returns the backlight power. 0 screen is off, 1 screen is on. */
  int blPower () const;

  /*! Returns the backlight brigthness [0, 255] */
 int brigthness () const;

  /*! Sets the backlight power. 0 screen is off, 1 screen is on. */
  bool setBlPower (int value);

  /*! Sets the backlight brigthness [0, 255] */
  bool setBrigtness(int value);

  /*! Returns true if the official screen is connected. */
  bool backlightConnected () const;

private :
  /*! Returns the backlight file content.
   * \param fileName: bl_power or brightness
   * \return The content of the file.
   */
  QByteArray read (QString const & fileName) const;

  /*! Write to the backlight file.
   * \param fileName: bl_power or brightness
   * \param value: [0,1] for bl_power, [0,255] for brightness.
   * \return True in case of success.
   */
  bool write (QString const & fileName, int value);

private :
  /*! The Raspberry PI 3 backlight screen folder. */
  QString m_folder = "/sys/class/backlight/rpi_backlight/";
};

#endif // RPI3_HPP
