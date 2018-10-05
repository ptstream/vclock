#ifndef LOCALISATION_HPP
#define LOCALISATION_HPP

#include <QDialog>

namespace Ui {
  class CLocalisation;
}

/*! \brief This class is avalaible to enter a town and a country. */
class CLocalisation : public QDialog
{
public:
  /*! Constructor.
   * \param town: The town at entry.
   * \param town: The country at entry.
   */
  CLocalisation (QString const & town, QString const & country, QWidget* parent = nullptr);

  /*! Destructor. */
  virtual ~CLocalisation ();

  /*! Returns the selected zip code. */
  QString zipCode () const;

  /*! Returns the selected town. */
  QString town () const;

public slots :
  /*! Accept slot. */
  void okClicked ();

  /*! Resets town and country. */
  void noLocClicked ();

  /*! Reject slot. */
  void closeClicked ();

private :
  Ui::CLocalisation* ui; //!< The UI definition.
};

#endif // LOCALISATION_HPP
