#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "nixie.hpp"
#include "../vtools/webradio.hpp"
#include <QMainWindow>
#ifdef QTMULTIMEDIA_DEFINED
#include <QMediaPlayer>
#else
#include "../vtools/mediaplayer.hpp"
#endif

class QTranslator;
class QToolButton;
class CAlarm;
class CTimer;
class CTempSensorInterface;
class CWeatherInterface;
class CBrightnessInterface;
class CBrightnessSensorInterface;
class QNetworkAccessManager;
class CBrightness;

namespace Ui {
  class CMainWindow;
}

/*! The main window. */
class CMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  /*! Opntion indices. */
  enum EOption { Display, //!< Display type.
                 TubeNumber, //!< Number of tubes.
                 Degree, //!< Degree Celcius or Fahrenheit.
                 Hour, //!< 24 or 12 hours format
                 Date, //!< Complet or simplified format.
                 Weather, //!< Activate or disable weather.
                 Forecasts, //!< Activate or disable forecasts.
                 Table, //!< Show ot hide table.
                 Pencil, //!< Show ot hide pencil.
                 Diaporama, //!< Activate or disable diaporama.
                 BackgroundUniformColor, //!< Activate or disable uniform background color.
                 Player, //!< External player name.
                 Move, //!< Activate or disable motion.
                 AutoBrigthness, //!< Automatic brightness management.
                 HightBrigthness, //!< Software hight brightness.
                 AboveMediumBrigthness, //!< Software above medium brightness.
                 MediumBrigthness, //!< Software medium brightness.
                 BelowMediumBrigthness, //!< Software below medium brightness.
                 LowBrigthness, //!< Software low brightness.
                 LastOption //!< Internal used.
               };

  /*! OpenGl and main window parameters. */
  struct SOpenGlParams
  {
    int m_samples = 8; //!< OpenGL samples if avalaible.
    int m_depthBufferSize = 16; //!< OpenGL depth buffer resolution.
    QRect m_rc; //!< Main window rectangle.
    Qt::WindowStates m_ws = Qt::WindowNoState; //!< Main window states.
    bool m_maxWindowSize = false; //!< Main window is in max size.
    bool m_showTitle = true; //!< Activate or disable main window title bar.
  };

  /*! Constructor. */
  explicit CMainWindow (QWidget *parent = 0);

  /*! Destructor. */
  ~CMainWindow ();

  /*! Restore the main window geometry. */
  void restoreGeometry (QRect const & rc, Qt::WindowStates ws);

  /*! Read open parameters from the ini file. */
  static void openGLConfig (SOpenGlParams& params);

private slots :
  /*! Front view. */
  void frontView ();

  /*! Top view. */
  void topView ();

  /*! Left view. */
  void leftView ();

  /*! Right view. */
  void rightView ();

  /*! Perspective view. */
  void perspective ();

  /*! Sets the display type. */
  void display ();

  /*! Sets the number of tubes. Available only for clock and fast counter. */
  void tubeNumber ();

  /*! Sets temperature unit. */
  void degree ();

  /*! Sets 12 or 24 hours display. */
  void hour ();

  /*! Shows or hides the date. */
  void date ();

  /*! Shows or hides the weather. */
  void weather ();

  /*! Shows or hides the forecasts. */
  void forecasts ();

  /*! Shows or hides the table. */
  void showHideTable ();

  /*! Shows or hides the pencil. */
  void showHidePencil ();

  /*! Shows or hides the webradio player. */
  void showHidePlayer ();

  /*! Stops or starts the diaporama. */
  void stopStartDiaporama ();

  /*! Enables or disables the uniform color background. */
  void forceReleaseBackgroungUniform ();

  /*! Enables or disables the motion. */
  void move ();

  /*! Activates the thermometer mode. */
  void thermometer ();

  /*! Activates the clock mode. */
  void clock ();

  /*! Activates the fastcounter mode. */
  void fastCounter ();

  /*! Activates the test digits mode. */
  void digitTest ();

  /*! Activates the stopwatch mode. */
  void stopwatch ();

  /*! Activates the timer mode. */
  void timer ();

  /*! Sets and shows the fps. */
  void frameCount (float count);

  /*! The clock display has changed. */
  void clockChanged ();

  /*! The weather display has changed. */
  void updateWeather ();

  /*! The user has clicked. */
  void glWidgetClicked (int x, int y);

  /*! Enable/disbale the automatic brightness.
   * If automatic brightness is enable, the brightness
   * is set to m_hightBrightness.
   */
  void autoBrightness ();

  /*! Sets the global brightness to hight. */
  void brightness0 ();

  /*! Sets the global brightness to medium hight. */
  void brightness1 ();

  /*! Set the global brightness to meium. */
  void brightness2 ();

  /*! Set the global brightness to medium low. */
  void brightness3 ();

  /*! Sets the global brightness to low. */
  void brightness4 ();

  /*! Stopwatch or timer state toggles to start or pause. */
  void on_m_swStartPause_clicked ();

  /*! Stopwatch resets lap time. */
  void on_m_swTourReset_clicked ();

  /*! About to show main menu. */
  void aboutToShowMainMemu ();

  /*! About to show sleep menu. */
  void aboutToShowSleepMemu ();

  /*! Halt the Raspberry PI. */
  void haltRPI ();

  /*! Launchs about dialog box. */
  void about ();

  /*! Pause or play the player. */
  void on_m_audioPlay_clicked ();

  /*! Mute or sound the player. */
  void on_m_mute_clicked ();

  /*! The volume has changed. */
  void on_m_volume_valueChanged (int value);

  /*! The player enters in sleeping mode. */
  void sleeping ();

  /*! Sleepind timer timeout. */
  void sleepTimeout ();

  /*! The brightness has changed. */
  void on_m_luminosity_valueChanged (int value);

  /*! Launch the localisation dialog. */
  void localisation ();

  /*! The weather logo has been clicked. */
  void on_m_weatherLogo_clicked ();

  /*! The webradio logo has been clicked. */
  void on_m_webRadiosLogo_clicked ();

  /*! Alarm is stopped. */
  void alarmStopped ();

  /*! Alarm repeat started. */
  void alarmRepeatStarted (unsigned index);

  /*! Alarm repeat restarted. */
  void alarmRestarted ();

  /*! Alarm or timer ring. */
  void ring ();

  /*! Alarm ringtone volume increased */
  void ringtoneIncreaseVolume ();

  /*! Launch the alarms dialog. */
  void alarms ();

  /*! Alarm button 1 clicked. */
   void on_m_a1_clicked ();

  /*! Alarm button 2 clicked. */
  void on_m_a2_clicked ();

  /*! Alarm button 3 clicked. */
  void on_m_a3_clicked ();
#ifdef QTMULTIMEDIA_DEFINED
  /*! Rigtone state changed (Playing or stopped state). */
  void ringtoneStateChanged (QMediaPlayer::State newState);
#else
  /*! Rigtone state changed (Playing or stopped state). */
  void ringtoneFinished ();
#endif

  /*! Timer button 1 clicked. */
  void on_m_t1_clicked ();

  /*! Timer button 2 clicked. */
  void on_m_t2_clicked ();

  /*! Timer button 3 clicked. */
  void on_m_t3_clicked ();

  /*! Brightness changed. Active on Raspberry only.
   * \param: value = 1 -> m_hightBrightness, 0 -> m_lowBrightness.
   */
  void brightnessChanged (float value);

protected :
  /*! Use for translation. */
  virtual void changeEvent (QEvent* event);

  /*! Close event manager. */
  virtual void closeEvent (QCloseEvent*);

private :
  void applyConfig (); //!< Apply config after read the ini file.
  void saveConfig (); //!< Save config in ini file.
  void setMainMenu (); //!< Creates the main menu.
  void setSleepingMenu (); //!< Creates the sleeping menu.
  void setWidgetColor (); //!< Update the widget color (QLabel and QSlider).
  void resetControls (QLayout* layout); //!< Resets controls of a layout.
  void resetControls (); //!< Resets controls
  void setBrightness (int index); //!< Sets the brightness.
  bool isOnWeather (int x, int y) const; //!< Click on the weather widgets (top-left).
  bool isOnDate (int x, int y) const; //!< Click on the date widgets (top-right).
  bool isOnPlayer (int x, int y) const; //!< Click on the player widgets (bottom).
  bool isOnLuminosity (int x, int y) const; //!< Click on the brightness widgets (right).
  bool isOnMode (int x, int y) const; //!< Click on mode part (bottom-left).
  bool isOnModeDayNight (int x, int y) const; //!< Click on day-night (top-left).
  void startWeather (); //!< Starts the weather.
  void initLocation (); //!< Initializes the location.
  void weatherSetHidden (bool hide); //!< Hides or shows the weather.
  void launchAlarms (); //!< Launchs alarms.
  void createRingtonePlayer (); //!< Creates the ringtone player.
  void alarmClicked (QObject* o); //!< Alarm button clicked.
  void timerClicked (QObject* sender); //!< Timer button clicked.
  void setMode (CMode::EType type); //!< Set the mode.
  void setHidden (QList<QToolButton*> const & tbs, bool hidden); //!< Shows or hides a list of buttons.
  void updateSleeping (); //!< Enables or disables sleeping button.
  void updateClockButtons (); //!< Update clock buttons.
  void updateTimerButtons (); //!< Update timer buttons.
  void updateAlarmButtons (); //!< Update alarm buttons.
  void updateStopwatchButons (); //!< Update stopwatch buttons.
  void setFontFamily (QString const & family); //!< Sets font family.
  float brightness () const; //!< Return the hardware or software brightness.
  void setSimpleMode (CMode::EType type); //!< Set fast counter or digit test mode.
  void stopStartupDigitsTest (); //!< Test digits end of cycle.

  static int sleepingInterval (QAction* action); //!< Returns the sleeping interval from an action.
  static QColor alarmIndexColor (QColor color); //!< Sets the color of the alarm index.
  static QPixmap addIndex (QPixmap const & pxm, unsigned index, QColor const & color, bool repeat = false); //!< Adds alarm index at the pixmap.
  static QPixmap addActive (QPixmap const & pxm, QColor const & color); //!< Adds 'A' character at the pixmap.
  static void addActive (QToolButton* tb, QColor const & color); //!< Adds 'A' character at the button.

private:
  Ui::CMainWindow* ui; //!< UI definition
  QAction* m_options[LastOption]; //!< Options actions.
  QTranslator* m_translator = nullptr; //!< Translator.
  QNetworkAccessManager* m_nam = nullptr; //!< The global QNetworkAccessManager.

  int m_dateFormat = 0; //!< Date format
  int m_cClockTubes = 4; //!< Number of tubes to display.
  bool m_h24 = true; //!< 24 or 12 hours clock.
  bool m_celcius = true; //!< Temperature units.
  float m_menuFontCoef = 0.0f;
  CMode::EType m_configType = CMode::Clock;
  QTimer m_startupTimer;

  // Web radios
  int m_stationNameLength = 60; //!< Maximum station name length.
  CWebRadio m_webRadios; //!< Webradios list.
  CWebRadiosInterface* m_webRadiosProvider = nullptr; //!< Webradios provider plugin.

  // Weather
  QTimer m_weatherTimer; //!< Weather timer.
  CWeatherInterface* m_weather = nullptr; //!< Weather provider plugin.
  int m_weatherInterval = 15 * 60 * 1000; //!< Waether detection interval (default 15 mn).
  QString m_locationUrl; //*! Location server URL.
  QString m_ipExternalUrl; //*! External IP server URL.
  int m_weatherRetry = 0; //*! Current weather fails.

  // Alarms
  QList<CAlarm*> m_alarms; //!< Alarms list.
  QUrl m_alarmRingtoneURL; //!< Alarm ringtone URL.
  QMediaPlayer* m_ringtonePlayer = nullptr; //!< Ringtone player.
  int m_ringtoneVolume = 1; //!< Ringtone volume.
  int m_ringtoneVolumeMax = 50; //!< Ringtone max volume for alarms.
  int m_ringtoneVolumeMin = 1; //!< Ringtone min volume for alarms.
  int m_ringtoneVolumeStep = 1; //!< Ringtone step volume for alarms.
  int m_ringtoneIncreaseInterval = 5000; //!< Ringtone increase interval.
  int m_ringtoneMS = 0; //!< Current ringtone duration in ms.
  int m_ringtoneDuration = 600000;  //!< Max ringtone duration in ms (default 10mn).
  QTimer m_ringtoneVolumeTimer; //!< Ringtone volume timer.

  // Timers
  int m_prevType = CMode::Clock; //!< Previous type update before change.
  QUrl m_timerRingtoneURL; //!< Timer ringtone URL.
  QList<CTimer*> m_timers; //!< Timers Timers.
  QTimer m_sleepingTimer; //!< Sleeping timer.
  int m_sleepingInterval = 0; //!< Sleeping interval.

  // Internal temperature.
  CTempSensorInterface* m_ambiantTemperatureSensor = nullptr;  //!< Temperature plugin.
  int m_countClockChanged = 0; //!< Clock changed in thermometer mode.

  // Brightness
  float m_lightAttenations[5] = { 1.0f, 1.5f, 2.0f, 3.0f, 4.0f }; //!< light attenuations
  CBrightness* m_brightness = nullptr; //!< Hardware brightness.
  float m_hightBrightness = 1.0f; //!< Hardware hight brightness.
  float m_lowBrightness = 0.2f; //!< Hardware low brightness.
  CNixie::EAnodeBrightness m_currentAnodeBrightness = CNixie::BMedium; // Anode brightness.
  CBrightnessInterface* m_brightnessPlugin = nullptr; //!< Hardware brightness plugin.
  CBrightnessSensorInterface* m_brightnessSensorPlugin = nullptr; //!< Sensor brightness plugin.

  static QColor m_primaryColor; //!< Primary color (255, 128, 25).
};

#endif // MAINWINDOW_HPP
