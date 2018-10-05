#include "webradio.hpp"
#include "../vtools/webradiosinterface.hpp"
#include <QVariant>

CWebRadio::CWebRadio (QObject* parent) : QMediaPlayer (parent)
{
  setVolume (25);
}

void CWebRadio::add (CStation const & station)
{
  m_stations.push_back (station);
  std::sort (m_stations.begin (), m_stations.end (),
             [] (CStation const & s1, CStation const & s2) { return s1.name () < s2.name (); });
}

void CWebRadio::del (QList<int> indices)
{
  std::sort (indices.begin (), indices.end (), [] (int i1, int i2) -> bool { return i1 > i2; });
  for (int index : indices)
  {
    m_stations.removeAt (index);
  }
}

QString CWebRadio::currentName (int len) const
{
  QString name = m_currentIndex == -1 || m_stations.isEmpty () ? QString () : m_stations[m_currentIndex].name ();
  if (len != -1 && name.length () > len)
  {
    name.truncate (len);
  }

  return name;
}

QUrl CWebRadio::currentLogo () const
{
  QUrl url;
  if (m_currentIndex != -1)
  {
    url = m_stations[m_currentIndex].logo ();
  }

  return url;
}

void CWebRadio::setCurrentIndex (int index)
{
  if (index >= 0 && index < m_stations.size () && media () != m_stations[index].url ())
  {
    setMedia (m_stations[index].url ());
    m_currentIndex = index;
  }
}

QStringList CWebRadio::names () const
{
  QStringList names;
  names.reserve (m_stations.size ());
  for (CStation const & station : m_stations)
  {
    names.push_back (station.name ());
  }

  return names;
}

int CWebRadio::index (QUrl const & url)
{
  int                              index = -1;
  CStation                         station (QString (), QVariant (), QUrl (), url);
  QVector<CStation>::const_iterator itEnd   = m_stations.cend ();
  QVector<CStation>::const_iterator itBegin = m_stations.cbegin ();
  QVector<CStation>::const_iterator it      = std::find (itBegin, itEnd, station);
  if (it != itEnd)
  {
    index = it - itBegin;
  }

  return index;
}

#ifndef QTMULTIMEDIA_DEFINED
void CWebRadio::setPlayerProgram (QString const & player)
{
  QMediaPlayer::setPlayerProgram (player);
}

void CWebRadio::setMixerProgram (QString const & mixer)
{
  QMediaPlayer::setMixerProgram (mixer);
}
#endif
