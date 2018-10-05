#include "version.hpp"

CVersion::CVersion (int major, int minor, int patch)
{
  m_values[Major] = major;
  m_values[Minor] = minor;
  m_values[Patch] = patch;
}

QList<QByteArray> CVersion::toByteArrayList () const
{
  QList<QByteArray> versions;
  versions.reserve (3);
  versions << QByteArray::number (m_values[Major])
           << QByteArray::number (m_values[Minor])
           << QByteArray::number (m_values[Patch]);
  return versions;
}

CVersion::CVersion (QByteArray const & version)
{
  fromByteArray (version);
}

CVersion::CVersion (QList<QByteArray> const & versions)
{
  fromByteArrayList (versions);
}

QByteArray CVersion::toByteArray () const
{
  QList<QByteArray> versions = toByteArrayList ();
  return versions[0] + '.' + versions[1] + '.' + versions[2];
}

void CVersion::fromByteArrayList (QList<QByteArray> const & versions)
{
  m_values[Major] = m_values[Minor] = m_values[Patch] = 0;
  for (unsigned i = 0; i < sizeof (m_values) / sizeof (int); ++i)
  {
    m_values[i] = versions[i].toInt ();
  }
}

void CVersion::fromByteArray (QByteArray const & version)
{
  QList<QByteArray> versions = version.split ('.');
  fromByteArrayList (versions);
}

bool CVersion::isCompatible (CVersion const & other) const
{
  bool valid = true;
  for (unsigned i = 0; i < sizeof (m_values) / sizeof (int) && valid; ++i)
  {
    valid = other.m_values[i] >= m_values[i];
  }

  return valid;
}

bool CVersion::isCompatible (QByteArray const & other) const
{
  CVersion vo (other);
  return isCompatible (vo);
}

bool CVersion::isCompatible (QList<QByteArray> const & other) const
{
  CVersion vo (other);
  return isCompatible (vo);
}

void CVersion::version (int values[3])
{
  values[Major] = m_values[Major];
  values[Minor] = m_values[Minor];
  values[Patch] = m_values[Patch];
}

void CVersion::setVersion (int values[3])
{
  m_values[Major] = values[Major];
  m_values[Minor] = values[Minor];
  m_values[Patch] = values[Patch];
}

void CVersion::setVersion (int major, int minor, int patch)
{
  m_values[Major] = major;
  m_values[Minor] = minor;
  m_values[Patch] = patch;
}
