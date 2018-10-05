#ifndef DISPLAYMANAGER_HPP
#define DISPLAYMANAGER_HPP

#include "nixie.hpp"
#include <QDialog>

namespace Ui {
  class CDisplayManager;
}

/*! \brief The display manager.
 * Actually nixie tubes, text, LCD and Leds are implemented.
 */
class CDisplayManager : public QDialog
{
public:
  /*! Constructor.
   * \param type: The type of display.
   * \param fontFamily: The font family to show texts.
   * \param parent: The parent widget.
   */
  CDisplayManager (CNixie::EDisplay type, QString const & fontFamily, QWidget* parent = nullptr);

  /*! Returns the font family. */
  QString fontFamily () const;

  /*! Returns the current display type. */
  CNixie::EDisplay displayType () const;

protected :
  /*! The system font checkbox state. */
  void systemToggled (bool checked);

private :
  Ui::CDisplayManager* ui; //!< The UI definition.
};

#endif // DISPLAYMANAGER_HPP
