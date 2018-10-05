#ifndef VERSION_HPP
#define VERSION_HPP

#include "vtools_globals.h"
#include <QList>
#include <QByteArray>

/*! \brief This class is the version manager.
 * It is essentially used by plugins. The default value is 0.0.0.
 * The version is defined by 3 integer values:
 * \li The major version
 * \li The minor version.
 * \li The patch version.
 */
class VTOOLSSHARED_EXPORT CVersion
{
public :
  /*! Version type. */
  enum EType { Major, //!< The major version
               Minor, //!< The minor version.
               Patch, //!< The patch version.
               Last //!< Internal use only.
             };

  /*! Constructor. */
  CVersion () { m_values[Major] = m_values[Minor] = m_values[Patch] = 0; }

  /*! Constructor. */
  CVersion (int major, int minor = 0, int patch = 0);

  /*! Constructor. */
  CVersion (QList<QByteArray> const & versions);

  /*! Copy constructor. */
  CVersion (QByteArray const & version);

  /*! Destructor. */
  virtual ~CVersion () {}

  /*! Returns the byte array version.
   * The form is major.minor.patch (e.g. 1.1.2).
   */
  QByteArray toByteArray () const;

  /*! Returns an array list of each version types. */
  QList<QByteArray> toByteArrayList () const;

  /*! Sets version from a byte array.
   * The form is major.minor.patch (e.g. 1.1.2).
   */
  void fromByteArray (QByteArray const & version);

  /*! Sets an array list of each version types. */
  void fromByteArrayList (QList<QByteArray> const & versions);

  /*! Returns true if other version is is compatible with this version.
   * The versions are compatible if:
   * \li major versions of other is less equal this major version and
   * \li minor versions of other is less equal this minor version and
   * \li patch versions of other is less equal this patch version.
   */
  bool isCompatible (CVersion const & other) const;

  /*! Returns true if other version is is compatible with this version.
   * Other is an byte array of each version types.
   * The versions are comptaible if:
   * \li major versions of other is less equal this major version and
   * \li minor versions of other is less equal this minor version and
   * \li patch versions of other is less equal this patch version.
   */
  bool isCompatible (QByteArray const & other) const;

  /*! Returns true if other version is is compatible with this version.
   * Other is an array list of each version types.
   * The versions are comptaible if:
   * \li major versions of other is less equal this major version and
   * \li minor versions of other is less equal this minor version and
   * \li patch versions of other is less equal this patch version.
   */
  bool isCompatible (QList<QByteArray> const & other) const;

  /*! Returns the version value from a type. */
  int version (EType type) const { return m_values[type]; }

  /*! Returns the versions. */
  void version (int values[3]);

  /*! Sets the version value for a type. */
  void setVersion (EType type, int value) { m_values[type] = value; }

  /*! Sets the versions. */
  void setVersion (int values[3]);

  /*! Sets the versions. */
  void setVersion (int major, int minor = 0, int patch = 0);

protected :
  int m_values[Last]; //!< Value for each type.
};

#endif // VERSION_HPP
