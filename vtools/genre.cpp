#include "genre.hpp"
#include <QVariant>

struct SGenreData : public QSharedData
{
  SGenreData () {}
  SGenreData (SGenreData const & other);

  QString m_name;
  QVariant m_id;
};

SGenreData::SGenreData (SGenreData const & other) : QSharedData (other),
                    m_name (other.m_name),
                    m_id (other.m_id)
{
}

CGenre::CGenre () : m_d (new SGenreData)
{
}

CGenre::CGenre (QString const & name, QVariant const & id) : m_d (new SGenreData)
{
  setName (name);
  setId (id);
}

CGenre::CGenre (CGenre const & rhs) : m_d (rhs.m_d)
{
}

CGenre& CGenre::operator = (CGenre const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CGenre::~CGenre ()
{
}

QString const & CGenre::name () const
{
  return m_d->m_name;
}

QVariant const & CGenre::id() const
{
  return m_d->m_id;
}

void CGenre::setName (QString const & value)
{
  m_d->m_name = value;
}

void CGenre::setId (QVariant const & value)
{
  m_d->m_id = value;
}

