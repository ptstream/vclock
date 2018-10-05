#ifndef ABOUT_HPP
#define ABOUT_HPP

#include <QDialog>

namespace Ui {
  class CAbout;
}

/*! \brief The about dialog box manager. */
class CAbout : public QDialog
{
public:
  /*! The logo index. */
  enum ELogoIndex { weather, //!< By default Yahoo weather.
                    radios,  //!< By default Shoutcast.
                    qt //!< Qt
                  };

  /*! Constructor. */
  CAbout (QVector<QUrl> const & logos, QWidget* parent = nullptr);

  /*! Destructor. */
  ~CAbout ();

public slots :
  /*! Click on weather button. */
  void weatherLogoClicked ();

  /*! Click on webradios button. */
  void webRadiosLogoClicked ();

  /*! Click on Qt button. */
  void qtLogoClicked ();

private :
  Ui::CAbout* ui; //!< The ui manager.
  QVector<QUrl> const & m_logos; //!< The logo urls for providers.
};

#endif // ABOUT_HPP
