#ifndef STATION_HPP
#define STATION_HPP

#include "vtools_globals.h"
#include <QSharedDataPointer>
#include <QUrl>

class SStationData;
class QVariant;

/*! \brief This class defines a Webradio station.
 */
class VTOOLSSHARED_EXPORT CStation
{
public:
  /*! Constructor. */
  CStation ();

  /*! Constructor. */
  CStation (QString const & name, QVariant const & id, QUrl const & logo = QUrl (),
            QUrl const & url = QUrl ());

  /*! Copy constructor. */
  CStation (CStation const & rhs);

  /*! Equal operator. */
  CStation& operator = (CStation const & rhs);

  /*! Destructor. */
  ~CStation ();

  /*! Equality operator. */
  bool operator == (CStation const & other) const;

  /*! Returns the station name. */
  QString const & name () const;

  /*! Returns the station identifier. */
  QVariant const & id () const;

  /*! Returns the station logo url. */
  QUrl const & logo () const;

  /*! Returns the station tunein url. */
  QUrl const & url () const;

  /*! Sets the station name. */
  void setName (QString const & value);

  /*! Sets the station identifier. */
  void setId (QVariant const & value);

  /*! Sets the station logo url. */
  void setLogo (QUrl const & value);

  /*! Sets the station tunein url. */
  void setUrl (QUrl const & value);

private:
  QSharedDataPointer<SStationData> m_d; //!< Station data.
};

#endif // STATION_HPP
