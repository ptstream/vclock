#ifndef GENRE_HPP
#define GENRE_HPP

#include "vtools_globals.h"
#include <QSharedDataPointer>
#include <QString>

class SGenreData;
class QVariant;

/*! \Brief The music genre container. */
class VTOOLSSHARED_EXPORT CGenre
{
public:
  /*! Constructor. */
  CGenre ();

  /*! Constructor with name and identifier. */
  CGenre (QString const & name, QVariant const & id);

  /*! Copy constructor. */
  CGenre (CGenre const & rhs);

  /*! Assign operator. */
  CGenre& operator = (CGenre const & rhs);

  /*! Destructor. */
  ~CGenre ();

  /*! Returns the name of genre. */
  QString const & name () const;

  /*! Returns the identifier of genre. */
  QVariant const & id () const;

  /*! Sets the name of genre. */
  void setName (QString const & value);

  /*! Sets the identifier of genre. */
  void setId (QVariant const & value);

private:
  QSharedDataPointer<SGenreData> m_d; // Genre data.
};

#endif // GENRE_HPP
