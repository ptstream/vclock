#include "about.hpp"
#include "../vtools/helper.hpp"
#include "ui_about.h"
#include <QUrl>
#include <QDesktopServices>

CAbout::CAbout (QVector<QUrl> const & logos, QWidget* parent) : QDialog (parent),
            ui (new Ui::CAbout), m_logos (logos)
{
  setWindowFlags (windowFlags () & (~Qt::WindowContextHelpButtonHint));
  ui->setupUi (this);
  connect (ui->m_close, &QPushButton::clicked, this, &CAbout::close);
  connect (ui->m_weatherLogo, &QToolButton::clicked, this, &CAbout::weatherLogoClicked);
  connect (ui->m_webRadiosLogo, &QToolButton::clicked, this, &CAbout::webRadiosLogoClicked);
  connect (ui->m_qt, &QToolButton::clicked, this, &CAbout::qtLogoClicked);
}

CAbout::~CAbout ()
{
  delete ui;
}

void CAbout::weatherLogoClicked ()
{
  if (!m_logos.isEmpty () && !m_logos[weather].isEmpty ())
  {
    QDesktopServices::openUrl (m_logos[weather]);
  }
}

void CAbout::webRadiosLogoClicked ()
{
  if (m_logos.size () > radios && !m_logos[radios].isEmpty ())
  {
    QDesktopServices::openUrl (m_logos[radios]);
  }
}

void CAbout::qtLogoClicked ()
{
  if (m_logos.size () > qt && !m_logos[qt].isEmpty ())
  {
    QDesktopServices::openUrl (m_logos[qt]);
  }
}
