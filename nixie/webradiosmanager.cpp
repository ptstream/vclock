#include "webradiosmanager.hpp"
#include "ui_webradiosmanager.h"
#include "../vtools/webradio.hpp"
#include "../vtools/webradiosinterface.hpp"
#include "../vtools/helper.hpp"
#include <QScrollBar>

CWebRadiosManager::CWebRadiosManager (CWebRadio& radio, CWebRadiosInterface* provider, QNetworkAccessManager* nam, QWidget* parent) :
     QDialog (parent), ui (new Ui::CWebRadiosManager), m_radio (radio), m_provider (provider), m_nam (nam)
{
  setWindowFlags (windowFlags () & (~Qt::WindowContextHelpButtonHint));
  ui->setupUi (this);
  m_stationBackup = m_radio.stations ();
  m_updateLogosTimer.setSingleShot (true);
  m_updateLogosTimer.setInterval (m_logoUpdateThresholdInterval);

  // Favorites
  loadFavorites ();

  // Get genres from webradios provider
  genresClicked ();
  ui->m_genre->setHidden (true);
  connect (ui->m_favorites, &QListWidget::itemDoubleClicked, this, &CWebRadiosManager::favoritesItemDoubleClicked);
  connect (ui->m_favorites, &QListWidget::itemSelectionChanged, this, &CWebRadiosManager::favoritesItemSelectionChanged);
  connect (ui->m_providerContent, &QListWidget::itemClicked, this, &CWebRadiosManager::providerContentItemClicked);
  connect (ui->m_providerContent, &QListWidget::itemDoubleClicked, this, &CWebRadiosManager::providerContentItemDoubleClicked);
  connect (ui->m_providerContent, &QListWidget::itemSelectionChanged, this, &CWebRadiosManager::providerContentItemSelectionChanged);
  connect (ui->m_genres, &QToolButton::clicked, this, &CWebRadiosManager::genresClicked);
  connect (ui->m_add, &QToolButton::clicked, this, &CWebRadiosManager::addClicked);
  connect (ui->m_remove, &QToolButton::clicked, this, &CWebRadiosManager::removeClicked);
  connect (ui->m_play, &QToolButton::clicked, this, &CWebRadiosManager::play);
  connect (ui->m_search, &QToolButton::clicked, this, &CWebRadiosManager::search);
  connect (ui->buttonBox, &QDialogButtonBox::rejected, this, &CWebRadiosManager::cancel);
  connect (ui->buttonBox, &QDialogButtonBox::accepted, this, &CWebRadiosManager::ok);
  connect (ui->m_providerContent->verticalScrollBar (), &QScrollBar::valueChanged, this, &CWebRadiosManager::scrollBarValueChanged);
  connect (&m_updateLogosTimer, &QTimer::timeout, this, &CWebRadiosManager::logoTimeout);
}

CWebRadiosManager::~CWebRadiosManager ()
{
  delete ui;
}

void CWebRadiosManager::favoritesItemDoubleClicked (QListWidgetItem* item)
{
  m_selectedRow = ui->m_favorites->row (item);
  accept ();
}

void CWebRadiosManager::favoritesItemSelectionChanged ()
{
  bool                    enable = false;
  QList<QListWidgetItem*> items  = ui->m_favorites->selectedItems ();
  if (!items.isEmpty ())
  {
    m_selectedRow = ui->m_favorites->row (items.first ());
    enable        = true;
  }
  else
  {
    m_selectedRow = -1;
  }

  ui->m_remove->setEnabled (enable);
}

void CWebRadiosManager::providerContentItemSelectionChanged ()
{
  bool enable = ui->m_genre->isVisible () && ui->m_providerContent->count () != 0;
  ui->m_play->setEnabled (enable);
  ui->m_add->setEnabled (enable);
}

void CWebRadiosManager::providerContentItemClicked (QListWidgetItem* item)
{
  if (item->type () == TGenre)
  {
    QApplication::setOverrideCursor (Qt::WaitCursor);
    QString                 text     = item->text ();
    QVector<CGenre> const & genres   = m_provider->genres ();
    int                     row      = ui->m_providerContent->row (item);
    QVariant const &        id       = genres[row].id ();
    QVector<CStation> const & stations = m_provider->stations (id);
    ui->m_providerContent->clear ();
    QIcon icon (":/icons/radio.png");
    m_blockUpdateLogo = true;
    for (CStation const & station : stations)
    {
      auto stationItem = new QListWidgetItem (station.name (), ui->m_providerContent, TStation);
      stationItem->setIcon (icon);
    }

    int cStations = stations.size ();
    m_icons.resize (cStations);
    m_icons.fill (false);
    QString genre = text + QString (" (%1)").arg (cStations);
    ui->m_genre->setVisible (true);
    ui->m_genre->setText (genre);
    m_blockUpdateLogo = false;
    updateLogos ();
    QApplication::restoreOverrideCursor ();
  }
}

void CWebRadiosManager::providerContentItemDoubleClicked (QListWidgetItem* item)
{
  if (item->type () == TStation)
  {
    m_updateLogosTimer.stop ();
    int row = ui->m_providerContent->row (item);
    if (row != -1)
    {
      QVector<CStation> const & stations = m_provider->stations ();
      CStation                  station  = stations[row];
      QUrl                      url      = m_provider->tunein (station.id (), m_nam);
      station.setUrl (url);
      m_radio.add (station);
      row = m_radio.index (url);
      if (row != -1)
      {
        loadFavorites ();
        item = ui->m_favorites->item (row);
        if (item != nullptr)
        {
          item->setSelected (true);
          m_temp = true;
          ok ();
        }
      }
    }
  }
}

void CWebRadiosManager::updateLogos ()
{
  if (!m_blockUpdateLogo)
  {
    m_updateLogosTimer.stop ();
    int   cItems       = ui->m_providerContent->count ();
    QRect viewportRect = ui->m_providerContent->viewport ()->rect ();
    for (int iItem = 0; iItem < cItems; ++iItem)
    {
      if (!m_icons[iItem])
      {
        QListWidgetItem* item     = ui->m_providerContent->item (iItem);
        QRect            itemRect = ui->m_providerContent->visualItemRect (item);
        if (viewportRect.intersects (itemRect))
        {
          m_blockUpdateLogo = true;
          QIcon icon        = logo (iItem);
          m_blockUpdateLogo = false;
          if (!icon.isNull ())
          {
            item->setIcon (icon);
          }

          m_icons[iItem] = true;
          m_updateLogosTimer.setInterval (m_logoUpdateRepeatInterval);
          m_updateLogosTimer.start ();
          break;
        }
      }
    }
  }
}

void CWebRadiosManager::loadFavorites (QUrl const & url)
{
  ui->m_favorites->clear ();
  QVector<CStation> const & stations = m_radio.stations ();
  QIcon                     defIcon (":/icons/radio.png");
  for (CStation const & station : stations)
  {
    auto            item    = new QListWidgetItem (station.name (), ui->m_favorites);
    QUrl const &     iconUrl = station.logo ();
    QIcon            icon    = !iconUrl.isValid () ? ::logo (iconUrl, m_nam) : defIcon;
    item->setIcon (icon);
    if (station.url () == url)
    {
      item->setSelected (true);
    }
  }
}

void CWebRadiosManager::genresClicked ()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);
  m_updateLogosTimer.stop ();
  ui->m_providerContent->clear ();
  ui->m_genre->setVisible (false);
  QVector<CGenre> const & genres = m_provider->genres ();
  for (CGenre const & genre : genres)
  {
    new QListWidgetItem (genre.name (), ui->m_providerContent, TGenre);
  }

  QApplication::restoreOverrideCursor ();
}

int CWebRadiosManager::addClicked ()
{
  int                     row   = -1;
  QList<QListWidgetItem*> items = ui->m_providerContent->selectedItems ();
  if (!items.isEmpty ())
  {
    row                                = ui->m_providerContent->row (items.first ());
    QVector<CStation> const & stations = m_provider->stations ();
    CStation                  station  = stations[row];
    QUrl                      url      = m_provider->tunein (station.id ());
    station.setUrl (url);
    m_radio.add (station);
    loadFavorites (url);
  }

  return row;
}

void CWebRadiosManager::removeClicked ()
{
  QList<QListWidgetItem*> items = ui->m_favorites->selectedItems ();
  if (!items.isEmpty ())
  {
    QList<int> rows;
    rows.reserve (items.size ());
    for (QListWidgetItem* item : items)
    {
      int row = ui->m_favorites->row (item);
      rows.push_back (row);
    }

    m_radio.del (rows);
    loadFavorites ();
  }
}

void CWebRadiosManager::cancel ()
{
  m_radio.stations () = m_stationBackup;
  reject ();
}

void CWebRadiosManager::ok ()
{
  QList<QListWidgetItem*> items = ui->m_favorites->selectedItems ();
  m_selectedRow                 = items.isEmpty () ? -1 : ui->m_favorites->row (items.first ());
  accept ();
}

void CWebRadiosManager::scrollBarValueChanged (int)
{
  if (ui->m_genres->isVisible () && ui->m_providerContent->count () != 0)
  {
    m_updateLogosTimer.start ();
  }
}

void CWebRadiosManager::logoTimeout ()
{
  updateLogos ();
}

QIcon CWebRadiosManager::logo (int index)
{
  QVector<CStation> const & stations = m_provider->stations ();
  return ::logo (stations[index].logo (), m_nam);
}

void CWebRadiosManager::play ()
{
  QList<QListWidgetItem*> items = ui->m_providerContent->selectedItems ();
  if (!items.isEmpty ())
  {
    providerContentItemDoubleClicked (items.first ());
  }
}

void CWebRadiosManager::search ()
{
  QString text = ui->m_text->text ();
  if (!text.isEmpty ())
  {
    QApplication::setOverrideCursor (Qt::WaitCursor);
    ui->m_providerContent->clear ();
    QVector<CStation> stations = m_provider->stations (text);
    if (stations.size () > 1)
    {
      QString ltext = text.toLower ();
      QString ls1, ls2;
      ls1.reserve (50);
      ls2.reserve (50);
      auto compare = [&ltext, &ls1, &ls2] (CStation const & s1, CStation const & s2) -> bool
      {
        ls1 = s1.name ().toLower ();
        ls2 = s2.name ().toLower ();
        auto i1 = static_cast<unsigned>(ls1.indexOf (ltext));
        auto i2 = static_cast<unsigned>(ls2.indexOf (ltext));
        if (i1 == i2)
        {
          return ls1 < ls2;
        }

        return i1 < i2;
      };

      std::sort (stations.begin (), stations.end (), compare);
    };

    QIcon icon (":/icons/radio.png");
    m_blockUpdateLogo = true;
    for (CStation const & station : stations)
    {
      auto item = new QListWidgetItem (station.name (), ui->m_providerContent, TStation);
      item->setIcon (icon);
    }

    int cStations = stations.size ();
    m_icons.resize (cStations);
    m_icons.fill (false);
    m_blockUpdateLogo = false;
    ui->m_genre->setText (QString::number (cStations));
    updateLogos ();

    QApplication::restoreOverrideCursor ();
  }
}

void CWebRadiosManager::resizeEvent (QResizeEvent*)
{
  scrollBarValueChanged (0);
}

QIcon CWebRadiosManager::selectedLogo () const
{
  QIcon icon;
  if (m_selectedRow != -1)
  {
    QListWidgetItem* item = ui->m_favorites->item (m_selectedRow);
    if (item != nullptr)
    {
      icon = item->icon ();
    }
  }

  return icon;
}

