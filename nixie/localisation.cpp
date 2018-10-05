#include "localisation.hpp"
#include "ui_localisation.h"

CLocalisation::CLocalisation (QString const & town, QString const & country, QWidget* parent) : QDialog (parent),  ui (new Ui::CLocalisation)
{
  setWindowFlags (windowFlags () & (~Qt::WindowContextHelpButtonHint));
  ui->setupUi (this);
  ui->m_town->setText (town);
  ui->m_zipCode->setText (country);
  connect (ui->m_ok, &QPushButton::clicked, this, &CLocalisation::okClicked);
  connect (ui->m_noLoc, &QPushButton::clicked, this, &CLocalisation::noLocClicked);
  connect (ui->m_close, &QPushButton::clicked, this, &CLocalisation::closeClicked);
}

CLocalisation::~CLocalisation ()
{
  delete ui;
}

QString CLocalisation::zipCode () const
{
  return ui->m_zipCode->text ();
}

QString CLocalisation::town () const
{
  return ui->m_town->text ();
}

void CLocalisation::okClicked ()
{
  accept ();
}

void CLocalisation::noLocClicked ()
{
  ui->m_town->clear ();
  ui->m_zipCode->clear ();
  accept ();
}

void CLocalisation::closeClicked ()
{
  reject ();
}
