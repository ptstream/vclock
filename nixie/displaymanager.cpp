#include "displaymanager.hpp"
#include "ui_displaymanager.h"
#include "../vtools/helper.hpp"
#include <QScroller>
#include <QAbstractItemView>

CDisplayManager::CDisplayManager (CNixie::EDisplay type, QString const & fontFamily, QWidget* parent) :
  QDialog (parent), ui (new Ui::CDisplayManager)
{
  setWindowFlags (windowFlags () & (~Qt::WindowContextHelpButtonHint));
  ui->setupUi (this);
  switch (type)
  {
    case CNixie::Nixie :
      ui->m_nixie->setChecked (true);
      break;

    case CNixie::Text :
      ui->m_text->setChecked (true);
      break;

    case CNixie::LCD :
      ui->m_lcd->setChecked (true);
      break;

    case CNixie::Leds :
      ui->m_led->setChecked (true);
      break;

    default :
      break;
  }

  QString f = fontFamily;
  if (f.isEmpty ())
  {
    f = QApplication::font ().family ();
  }

  ui->m_fontFamily->setCurrentText (f);
  connect (ui->m_system, &QCheckBox::toggled, this, &CDisplayManager::systemToggled);

  if (touchScreenActivated ())
  {
    ui->m_fontFamily->setAttribute (Qt::WA_AcceptTouchEvents, true);
    ui->m_fontFamily->view ()->setVerticalScrollMode (QAbstractItemView::ScrollPerPixel);
    QScroller::grabGesture (ui->m_fontFamily, QScroller::TouchGesture);
  }
}

QString CDisplayManager::fontFamily () const
{
  return ui->m_fontFamily->currentText ();
}

CNixie::EDisplay CDisplayManager::displayType () const
{
  CNixie::EDisplay type = CNixie::Nixie;;
  if (ui->m_text->isChecked ())
  {
    type = CNixie::Text;
  }
  else if (ui->m_lcd->isChecked ())
  {
    type = CNixie::LCD;
  }
  else if (ui->m_led->isChecked ())
  {
    type = CNixie::Leds;
  }

  return type;
}

void CDisplayManager::systemToggled (bool checked)
{
  ui->m_fontFamily->setEnabled (!checked);
  if (checked)
  {
    ui->m_fontFamily->setCurrentText (QApplication::font ().family ());
  }
}
