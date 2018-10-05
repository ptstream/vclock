#ifndef WEBRADIOSMANAGER_HPP
#define WEBRADIOSMANAGER_HPP

#include "../vtools/station.hpp"
#include <QDialog>
#include <QTimer>

class QListWidgetItem;
class QNetworkAccessManager;
class CWebRadio;
class CWebRadiosInterface;

namespace Ui {
  class CWebRadiosManager;
}

/*! \brief The webradios manager. */
class CWebRadiosManager : public QDialog
{
public:
  enum EItemType { TGenre,
                   TStation
                 };

  /*! Constructor.
   * \param radio: The radio object to update.
   * \param provider: The plugin provider.
   * \param nam: A global QNetworkAccessManager.
   * \param parent: The widget parent.
   */
  CWebRadiosManager (CWebRadio& radio, CWebRadiosInterface* provider, QNetworkAccessManager* nam, QWidget* parent);

  /*! Destructor. */
  virtual ~CWebRadiosManager ();

  /*! Return true if the webradio is temporary. */
  bool isTemp () const { return m_temp; }

  /*! Returns the selected webradio. */
  int selectedRow () const { return m_selectedRow; }

  /*! Returns the selected logo. */
  QIcon selectedLogo () const;

protected slots :
  /*! Favorites item double clicked. */
  void favoritesItemDoubleClicked (QListWidgetItem *item);

  /*! Favorites selction changed. */
  void favoritesItemSelectionChanged ();

  /*! Provider item clicked. */
  void providerContentItemClicked (QListWidgetItem *item);

  /*! Provider item double clicked. */
  void providerContentItemDoubleClicked (QListWidgetItem *item);

  /*! Provider item selected change. */
  void providerContentItemSelectionChanged ();

  /*! Provider genres clocked. */
  void genresClicked ();

  /*! Adds a new station. */
  int addClicked ();

  /*! Removes a new station. */
  void removeClicked ();

  /*! Cancel slot. */
  void cancel ();

  /*! Ok slot. */
  void ok ();

  /*! Scroll bar provider changed. */
  void scrollBarValueChanged (int);

  /*! Logo timer timeout to update logo station. */
  void logoTimeout ();

  /*! Close an play a station. */
  void play ();

  /*! Search a station. */
  void search ();

protected :
  /*! Resize manager. */
  virtual void resizeEvent (QResizeEvent*);

private :
  /*! Loads the favorites list view. */
  void loadFavorites (QUrl const & url = QUrl ());

  /*! Returns the icon from a favorite index. */
  QIcon logo (int index);

  /*! Updates the logo station. */
  void updateLogos ();

private :
  Ui::CWebRadiosManager* ui; //!< UI definition
  CWebRadio& m_radio; //!< Radios to update.
  QVector<CStation> m_stationBackup; //!< Backup radios at entry.
  int m_selectedRow = -1; //!< Selected station.
  bool m_temp = false; //!< Defined like temporary.
  CWebRadiosInterface* m_provider; //!< Provider plugin.
  QNetworkAccessManager* m_nam; //!< A global QNetworkAccessManager
  QVector<bool> m_icons; //!< Define which logo station is updated.
  QTimer m_updateLogosTimer; //!< Update logo timer.
  int m_logoUpdateThresholdInterval = 500; //!< First update logo delay.
  int m_logoUpdateRepeatInterval = 10; //!< Successive update logo delay.
  bool m_blockUpdateLogo = true; //!< Block the logo update.
};

#endif // WEBRADIOSMANAGER_HPP
