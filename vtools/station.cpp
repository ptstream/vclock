#include "station.hpp"
#include <QUrl>
#include <QVariant>

struct SStationData : public QSharedData
{
  SStationData () {}
  SStationData (SStationData const & other);

  QString m_name;
  QVariant m_id;
  QUrl m_logo;
  QUrl m_url;
};

SStationData::SStationData (SStationData const & other) : QSharedData (other),
                    m_name (other.m_name),
                    m_id (other.m_id),
                    m_logo (other.m_logo),
                    m_url (other.m_url)
{
}

CStation::CStation () : m_d (new SStationData)
{
}

CStation::CStation (QString const & name, QVariant const & id, QUrl const & logo, QUrl const & url) : m_d (new SStationData)
{
  setName (name);
  setId (id);
  setLogo (logo);
  setUrl (url);
}

CStation::CStation (CStation const & rhs) : m_d (rhs.m_d)
{
}

CStation& CStation::operator = (CStation const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CStation::~CStation ()
{
}

bool CStation::operator == (CStation const & other) const
{
  return m_d->m_url == other.m_d->m_url;
}

QString const & CStation::name () const
{
  return m_d->m_name;
}

QVariant const & CStation::id () const
{
  return m_d->m_id;
}

QUrl const & CStation::logo () const
{
  return m_d->m_logo;
}

QUrl const & CStation::url () const
{
  return m_d->m_url;
}

void CStation::setName (QString const & value)
{
  m_d->m_name = value;
}

void CStation::setId (QVariant const & value)
{
  m_d->m_id = value;
}

void CStation::setLogo (QUrl const & value)
{
  m_d->m_logo = value;
}

void CStation::setUrl (QUrl const & value)
{
  m_d->m_url = value;
}

