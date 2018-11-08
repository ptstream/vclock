#ifndef NIXIE_HPP
#define NIXIE_HPP

#include "../vmode/mode.hpp"
#include "../vtools/diaporama.hpp"
#include "../vtools/lighting.hpp"
#include "../vtools/light.hpp"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QTimer>
#include <QTime>
#include <QMutex>

class QOpenGLTexture;

#define MAXDIGITS 6

/*! \brief This class is the main widget who manages the display of nixie tubes clock.
 * All parts of the device (clock, thermometer, ...) have a 3d representation given by a stl file.
 * The different lights are used to manipulate the lignting.
 * Only Nixie display is 3D, other representation are 2D.
 */
class CNixie : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  /*! Digit display type. */
  enum EDisplay { Nixie, //!< Digit display is an individual Nixie tube.
                  Text, //!< Digit display is a flat text.
                  LCD, //!< The digits have a representation of an LCD display.
                  Leds, //!< The digits have a representation of an LCD display made of individual led.
                  LastDisplay //!< Internal use.
                };

  /*! Mesh indicies. */
  enum EMesh { MZero,   //!< Lighted cathode zero
               MOne,    //!< Lighted cathode One
               MTwo,    //!< Lighted cathode Two
               MThree,  //!< Lighted cathode Three
               MFour,   //!< Lighted cathode Four
               MFive,   //!< Lighted cathode Five
               MSix,    //!< Lighted cathode Six
               MSeven,  //!< Lighted cathode Seven
               MEight,  //!< Lighted cathode Eight
               MNine,   //!< Lighted cathode Nine
               MTZero,  //!< Cathode off zero
               MTOne,   //!< Cathode off One
               MTTwo,   //!< Cathode off Two
               MTThree, //!< Cathode off Three
               MTFour,  //!< Cathode off Four
               MTFive,  //!< Cathode off Five
               MTSix,   //!< Cathode off Six
               MTSeven, //!< Cathode off Seven
               MTEight, //!< Cathode off Eight
               MTNine,  //!< Cathode off Nine
               MBackground, //!< Backgound rectangle
               MBulb, //!< Buble
               MAnode, //!< Anode
               MBase, //!< Tuble base
               MFixAnode, //!< Anode fixture
               MPins, //!< Tubes pins
               MFixDigits, //!< Digit fixture
               MTop, //!< Box top
               MCase, //!< Box case
               MTag, //!< Box tag
               MFixTop, //!< Box top fixture
               MFixTag, //!< Box tag fixture
               MSepBulb, //!< Neon bulb separator
               MSphere, //!< Neon emitter separator
               MSepBase, //!< Neon base separator
               MSepWires, //!< Neon wire separator
               MMinusBulb, //!< Neon minus sign
               MMinus, //!< Neon emitter sign
               MMinusWires, //!< Neon minus sign wire
               MMinusBase, //!< Neon minus sign base
               MAboutRect, //!< About rectangle
               MRubberFoot, //!< Rubber foot
               MTable, //!< Table
               MPencil, //!< Pencil body
               MPencil2, //!< Cut part of the pencil
               MPencil3, //!< Pencil lead
               MLastMesh //!< Internal used
             };

  /*! Textures indices. */
  enum ETexture { TBackground, //!< Background
                  TAbout, //!< About
                  TTable, //!< Table
                  TLast, //!< Internal used
                };

  /*! Part indices. */
  enum EPart { PBulb, //!< Nixie bulb
               PAnode, //!< Nixie anode
               PBase, //!< Nixie base
               PDigits, //!< Nixie cathodes
               PFixAnode, //!< Anode fixture
               PFixDigits, //!< Digits fixture
               PTop, //!< Box top
               PCase, //!< Box case
               PRubberFoot, //!< Rubber foot
               PTag, //!< Tag
               PFixTop, //!< Top fixture
               PFixTag, //!< Tag fixture
               PTable, //!<  Table
               PSepBulb, //!< Neon bulb
               PSphere, //!< Neon emitter
               PSepBase, //!< Neon base
               PPencil,  //!< Cut part of the pencil
               PPencil2, //!< Penci //!< Pencil lead l lead
               PPencil3, //!< Internal used
               PLastPart, //!< Internal used
             };

  /*! Part codes. This codes are used to hide or show the parts. */
  enum EPartCode { PCBulb       = 0x00000001, //!< Nixie bulb
                   PCAnode      = 0x00000002, //!< Nixie anode
                   PCBase       = 0x00000004, //!< Nixie base
                   PCDigits     = 0x00000008, //!< Nixie cathodes
                   PCFixAnode   = 0x00000010, //!< Anode fixture
                   PCFixDigits  = 0x00000020, //!< Digits fixture
                   PCTop        = 0x00000040, //!< Box top
                   PCCase       = 0x00000080, //!< Box case
                   PCRubberFoot = 0x00000100, //!< Rubber foot
                   PCTag        = 0x00000200, //!< Tag
                   PCFixTop     = 0x00000400, //!< Top fixture
                   PCFixTag     = 0x00000800, //!< Tag fixture
                   PCTable      = 0x00001000, //!<  Table
                   PCAbout      = 0x00002000, //!< About tag
                   PCPencil     = 0x00004000, //!< Cut part of the pencil
                   PCSepBulb    = 0x00008000, //!< Neon bulb
                   PCSphere     = 0x00010000, //!< Neon emitter
                   PCSepBase    = 0x00020000, //!< Neon base
                   PCLast       = 0x7fffffff, //!< Internal used
                 };

  /*! Anode brightness. */
  enum EAnodeBrightness { BLow, //!< Low
                          BMedium, //!< Medium
                          BHight //!< Hight
                        };

  /*! Section of this widget. Click on a part to make shortcut. */
  enum ESection { Top, //!< Top of widget
                  Right, //!< Right of widget
                  Bottom, //!< Bottom of widget
                  Left, //!< Left of widget
                  Center, //!< The remaining part
                };

  /*! OpenGL uniform variable location. */
  struct SOpenGLUniformVarLoc
  {
    int m_projMatrix = 0; //!< Projection matrix opengl location.
    int m_mvMatrix = 0; //!< Word matrix opengl location.
    int m_normalMatrix = 0; //!< Normal matrix (m_mvMatrix[3,3]) opengl location.

    int m_cameraPos = 0; //!< camera opengl location.
    int m_surfaceColor = 0; //!< surface color opengl location.
    int m_specularColor = 0; //!< specular color opengl location.
    int m_shininess = 0; //!< shininess opengl location.
    int m_digit = 0; //!< A digit in currently drawn.
    int m_numLights = 0; //!< Number of lights.
    int m_texActivated = 0; //!< A texture is activated.
  };

  /*! Constructor. */
  CNixie (QWidget* parent = nullptr);

  /*! Destructor. */
  virtual ~CNixie () override;

  // Display
  /*! Returns the current display type. */
  EDisplay display () const { return m_display; }

  /*! Sets the current display type. */
  void setDisplay (EDisplay display);

  /*! Sets the current font family. */
  void setFontFamily (QString const & family) { m_fontFamily = family; }

  /*! Returns the current font family. */
  QString const &  fontFamily () const { return m_fontFamily; }

  // Mode
  /*! Returns the current mode type. */
  CMode::EType type () const { return m_mode->type (); }

  /*! Sets the current mode type. */
  void setType (CMode::EType type, QVariant const & data = QVariant ());

  /*! Returns the current mode. */
  CMode* mode () { return m_mode; }

  /*! Returns the current mode. */
  CMode const * mode () const { return m_mode; }

  /*! Changes the VBO according of the current mode. */
  void changeVbo ();

  // Configuration
  /*! Sets the primary color. Actually it is an orange color RGB (255, 128, 25). */
  void setPrimaryColor (QColor const & color);

  /*! Initialize the default lightings for each part. */
  void initializeDefaultLightings ();

  /*! Sets the background texture. */
  void setBackgroundTexture (QString const & fileName) { m_bkgndTexture = fileName; }

  /*! Sets the about texture. */
  void setAboutTexture (QString const & fileName) { m_aboutTexture = fileName; }

  /*! Sets the table texture. */
  void setTableTexture (QString const & fileName) { m_tableTexture = fileName; }

  /*! Sets the background color. */
  void setBackgroundUniformColor (QColor const & color);

  /*! Sets the stl folder. */
  void setStlDirName (QString const & dirName) { m_stlDirName = dirName; }

  /*! Sets the anode brightness. */
  void setAnodeBrightness (EAnodeBrightness brightness);

  /*! Sets the diaporama images folder brightness. */
  void setDiaporamaImages (QStringList const & fileNames);

  /*! Sets the diaporama mode. */
  void setDiaporamaMode (CDiaporama::EMode mode) { m_diaporamaMode = mode; }

  /*! Sets the diaporama intervall between two images in ms. */
  void setDiaporamaInterval (int interval);

  /*! Sets the part code to hide or show the parts. */
  void setPartCode (int code) { m_partCode = static_cast<EPartCode>(code); }

  /*! Validates the motion of the device. */
  void setInMotion (bool move);

  /*! Returns true if the device is in motion. */
  bool inMotion () const { return m_move; }

  /*! Sets the motion interval. */
  void setMotionInterval (int interval) { m_motionTimer.setInterval (interval); }

  /*! Sets the mood ligth positions. */
  void setMoodLightPositions (float x, float y, float z);

   /*! Sets the mood ligth color. */
 void setMoodLightIntensities (float r, float g, float b);

  // Background
  /*! Returns true if the diaporama is active. */
  bool isDiaporamaActive () const;

  /*! Returns true if the background has an uniform color (the diaparama is stopped). */
  bool isBackgroungUniform () const { return m_forceBackgroundColor; }

  /*! Releases background uniform color. */
  void forceReleaseBackgroungUniform ();

  /*! Stop the diaporama. The current image is already displayed. */
  void stopStartDiaporama ();

  // Nixie tubes
  /*! Returns the number of nixie tubes used (3, 4 or 6). */
  int nixieTubeNumber () const { return m_cNixieTubes; }

  /*! Sets the number of nixie tubes to use (3, 4 or 6). */
  void setNixieTubeNumber (int count);

  // Part code
  /*! Returns true if a part is visible. */
  bool isVisiblePart (EPartCode code) const { return (m_partCode & code) != 0; }

  /*! Toggles the code of a part. */
  void reversePartCode (EPartCode mode);

  /*! Returns the part code. */
  quint32 partCode () const { return m_partCode; }

  // Attenuation
  /*! Sets the light attenuation [0, 1]. */
  void setLightAttenuation (float attenuation);

  /*! Returns the current light attenuation [0, 1]. */
  float lightAttenuation () const;

  // Redraw timer
  /*! Returns the readraw timer. */
  QTimer& redrawTimer () { return m_redrawTimer; }

  // Add a mesh position
  /*! Adds a nixie tube position.
   * \internal
   */
  void addPos (int index, QVector3D const & loc) { m_nixiePos[index].push_back (loc); }

  /*! Returns the nixie tube positions.
   * \internal
   */
  QVector<QVector<QVector3D>> const & tubePos () const { return m_nixiePos; }

  // Orientation
  /*! Sets the x axis orientation. 360° is represented by 360 * 16.*/
  void setXRot (int a) { m_xRot = a * 16; }

  /*! Sets the y axis orientation. 360° is represented by 360 * 16 */
  void setYRot (int a) { m_yRot = a * 16; }

  /*! Sets the z axis orientation. 360° is represented by 360 * 16 */
  void setZRot (int a) { m_zRot = a * 16; }

  // Lighting
  /*! Set the current part to change the lightings.
   * \internal
   */
  void setCurrentPart (EPart part) { m_currentPart = part; }

  /*! Set red component of the current part light.
   * \internal
   */
  void setRL (float value) { m_lightings[m_currentPart].m_light.setX (value); update (); }

  /*! Set green component of the current part light.
   * \internal
   */
  void setGL (float value) { m_lightings[m_currentPart].m_light.setY (value); update (); }

  /*! Set blue component of the current part light.
   * \internal
   */
  void setBL (float value) { m_lightings[m_currentPart].m_light.setZ (value); update (); }

  /*! Set red component of the current part specular.
   * \internal
   */
  void setRS (float value) { m_lightings[m_currentPart].m_specular.setX (value); update (); }

  /*! Set green component of the current part specular.
   * \internal
   */
  void setGS (float value) { m_lightings[m_currentPart].m_specular.setY (value); update (); }

  /*! Set blue component of the current part specular.
   * \internal
   */
  void setBS (float value) { m_lightings[m_currentPart].m_specular.setZ (value); update (); }

  /*! Set red component of the current part surface color.
   * \internal
   */
  void setRM (float value) { m_lightings[m_currentPart].m_surfaceColor.setX (value); update (); }

  /*! Set green component of the current part surface color.
   * \internal
   */
  void setGM (float value) { m_lightings[m_currentPart].m_surfaceColor.setY (value); update (); }

  /*! Set blue component of the current part surface color.
   * \internal
   */
  void setBM (float value) { m_lightings[m_currentPart].m_surfaceColor.setZ (value); update (); }

  /*! Returns red component of the current part light.
   * \internal
   */
  float rl () const { return m_lightings[m_currentPart].m_light.x (); }

  /*! returns green component of the current part light.
   * \internal
   */
  float gl () const { return m_lightings[m_currentPart].m_light.y (); }

  /*! Returns blue component of the current part light.
   * \internal
   */
  float bl () const { return m_lightings[m_currentPart].m_light.z (); }

  /*! returns red component of the current part specular.
   * \internal
   */
  float rs () const { return m_lightings[m_currentPart].m_specular.x (); }

  /*! Returns green component of the current part specular.
   * \internal
   */
  float gs () const { return m_lightings[m_currentPart].m_specular.y (); }

  /*! Returns blue component of the current part specular.
   * \internal
   */
  float bs () const { return m_lightings[m_currentPart].m_specular.z (); }

  /*! Returns red component of the current part surface color.
   * \internal
   */
  float rm () const { return m_lightings[m_currentPart].m_surfaceColor.x (); }

  /*! Returns green component of the current part surface color.
   * \internal
   */
  float gm () const { return m_lightings[m_currentPart].m_surfaceColor.y (); }

  /*! Returns blue component of the current part surface color.
   * \internal
   */
  float bm () const { return m_lightings[m_currentPart].m_surfaceColor.z (); }

  /*! Sets shininess of the current part.
   * \internal
   */
  void setS (float value) { m_lightings[m_currentPart].m_shininess = value; update (); }

  /*! Returns shininess of the current part.
   * \internal
   */
  float s () const { return m_lightings[m_currentPart].m_shininess; }

  /*! Sets alpha of the current part.
   * \internal
   */
  void setA (float value) { m_lightings[m_currentPart].m_alpha = value; update (); }

  /*! Returns alpha of the current part.
   * \internal
   */
  float a () const { return m_lightings[m_currentPart].m_alpha; }

  /*! Sets the current lightings at the default values. */
  void setDefaultLightings () { m_lightings = m_defaultLightings; update (); }

  /*! Enables the dynamic rotation. */
  void setDynamicRotationEnabled (bool enable) { m_dynamicRotation = enable; }

  /*! Returns true if dynamic rotation is enabled. */
  bool hasDynamicRotationEnabled () const { return m_dynamicRotation; }

  /*! Returns the section from pointer coordinates. */
  ESection clickedSection (int x, int y) const;

  /*! Sets the left bottom point to reverse motion direction. */
  void setMotionMin (QVector3D const & p);

  /*! Sets the right top point to reverse motion direction. */
  void setMotionMax (QVector3D const & p);

  /*! Sets the motion increment. */
  void setMotionIncr (QVector3D const & incs);

public slots:
  /*! Set the rotation angle.
   * \param angle: New angle value.
   * \param rot: Angle to change.
   */
  void setRotation (int angle, int& rot);

  /*! Free all allocated memory. */
  void cleanup ();

  /*! Update the digits and show the results. */
  void updateTubes ();

  /*! Update the background. */
  void updateBackground ();

signals :
  /*! Emmited in case of fast counter. */
  void frameCount (float);

  /*! Emmited when the clock changes. */
  void clockChanged ();

  /*! Emmited after click on the widget. */
  void glWidgetClicked (int x, int y);

protected:
  /*! Standard openGL initialization. */
  void initializeGL () override;

  /*! Standard openGL painting. */
  void paintGL () override;

  /*! Standard openGL resize. */
  void resizeGL (int width, int height) override;

  /*! Standard mouse press event. */
  void mousePressEvent (QMouseEvent *event) override;

  /*! Standard mouse release event. */
  void mouseReleaseEvent (QMouseEvent *event) override;

  /*! Standard mouse move event. */
  void mouseMoveEvent (QMouseEvent *event) override;

  /*! Paint the nixie device. */
  void paintNixieTubes ();

  /*! Paint the text display. */
  void paintText ();

  /*! Paint the LCD display. */
  void paintLCD ();

  /*! Paint the leds display. */
  void paintLeds ();

  /*! Sets the openGL vertex attributs. */
  void setupVertexAttribs ();

  /*! Read the stl files. */
  QVector<GLfloat> readMesh (QString const & fileName);

  /*! Allocate the vbos. */
  void allocateVbo  ();

  /*! Returns the adimensioned value of a dimension. */
  float removeDim (float x) { return x * m_reducingCoefficient; }

  /*! Returns the about rectangle. */
  QVector<GLfloat> aboutRectangle ();

  /*! Sets the texture name at index. */
  void setTexture (ETexture index, QString const & name);

  /*! Draw mesh. Use for refractoring. */
  inline void drawArrays (EMesh mesh, GLenum mode = GL_TRIANGLES);

  /*! Draw mesh. Use for refractoring. */
  inline void drawArrays (EMesh mesh, int start, int count, GLenum mode = GL_TRIANGLES);

  /*! Updates openGL matrices. Use for refractoring. */
  inline void updateMatrix ();

  /*! Updates openGL matrices. Use for refractoring. */
  inline void updateMatrixAndDraw (EMesh mesh, GLenum mode = GL_TRIANGLES);

  /*! Updates openGL matrices. Use for refractoring. */
  inline void updateMatrixAndDraw (EMesh mesh, float dx, float dy, float dz, GLenum mode = GL_TRIANGLES);

  /*! Returns openGL vertex attributes size in bytes. */
  static unsigned byteSize (unsigned cVertices) { return cVertices * sizeof(GLfloat) * 6; }

  /*! Returns the background rectangle. */
  static QVector<GLfloat> rectangle ();

private :
  EDisplay m_display = Nixie; //!< Display type (nixie, text, LCD, Leds).
  CMode* m_mode = nullptr; //!< Current mode (Clock, Thermometer...
  QVector3D m_primaryColor = QVector3D (1.0f, 0.54f, 0.1f);
  QColor m_bkgndUniformColor = Qt::black; //!< Uniform background color.
  QString m_bkgndTexture; //!< Background texture file.
  QString m_aboutTexture; //!< About tag texture file.
  QString m_tableTexture; //!< Table texture file.
  QString m_fontFamily; //!< Font family.

  QStringList m_diaporamaImages; //!< Liste of images for the diaporama.
  CDiaporama::EMode m_diaporamaMode = CDiaporama::CropImage;
  int m_diaporamaIndex = 0; //!< Current image index for the diaporama.
  QMutex m_diaporamaMutex; //!< Diaporama mutex. Image is update from another thread.
  int m_diaporamaInterval = 4000; //!< Diaporama interval (4s)

  int m_cNixieTubes = 0; //!< Current number of tubes.
  QString m_stlDirName; //!< Stls folder.
  bool m_minus = false; //!< Show of hide minus tube.

  QVector<QVector<QVector3D>> m_nixiePos; //!< Tubes positions (adimensioned).
  QVector<quint8> m_digits; //!< Active digits for each tube.

  QVector3D m_motionTrans; //!< Motion current move.
  QVector3D m_motionIncs = QVector3D (0.002f, 0.002f, 0.002f); //!< Motion increment
  QVector3D m_motionMin = QVector3D (-0.1f, -0.1f, -0.5f); //!< Left-bottom point to reverse motion direction.
  QVector3D m_motionMax = QVector3D (0.1f, 0.1f, 0.3f); //!< Right-top point to reverse motion direction.
  bool m_move = false; //!< True motion in progress.

  int m_xRot = 0; //!< X angle. Integer value represents 16 * 360°.
  int m_yRot = 0; //!< Y angle. Integer value represents 16 * 360°.
  int m_zRot = 0; //!< Z angle. Integer value represents 16 * 360°.
  QPoint m_lastPos; //!< Last curser position (dynamic rotation).
  QVector<QOpenGLBuffer> m_vbos; //!< OpenGL VBO.
  QVector<bool> m_validVbos; //!< No empty VBO.

  QOpenGLShaderProgram* m_nixieProgram = nullptr; //!< Shader tube program.
  QOpenGLShaderProgram* m_bkgndProgram = nullptr; //!< Shader background program.

  float m_fieldOfViewAngle = 45.0f; //!< Current field of view angle.
  int m_maxCounter = 10; //!< Max counter value when Counter or FastCounter is used.

  SOpenGLUniformVarLoc m_uVLocs; //!< OpenGL uniform variables locations.

  QMatrix4x4 m_proj; //!< Projection matrix. Define by perspective.
  QMatrix4x4 m_camera; //!< Camera matrix. Fix here.
  QMatrix4x4 m_world; //!< World matrix.

  QVector<int> m_cVertices; //!< Array of vertices for each part.
  QVector<QOpenGLTexture*> m_textures; //!< Array of textures/

  float m_bulbRadius = 0; //!< Bulb radius.
  float m_bulbHeight = 0; //!< Bulb height.

  /*! Adimensionnement coefficient.
   *  Generally 1.5f * (bulb height) * (tube number / 2 + 1);
   */
  float m_reducingCoefficient = 0; //!< Adimensionnement coefficient. 1.5f * (dMax - dMin) * (m_cNixieTubes / 2 + 1);

  EPart m_currentPart = PDigits; //!< Lighting changes for the current part..
  QVector<CLighting> m_lightings; //!< Current lighting values for each part. Use only when lighting is changed.
  QVector<CLighting> m_defaultLightings; //!< Default lighting values for each part.
  QVector4D m_iniMoodLightPositions[3]; //!< Mood light positions from ini file.
  QVector4D m_moodLightPositions[3]; //!< Mood light positions
  QVector3D m_moodLightIntensities; //!< Mood light intensities

  /*! Light position for each digit value in percent of bulb height. */
  float m_anodeLightPositions[10] = { 1.0f / 3.0f,
                                      1.0f / 3.0f,
                                      1.0f / 3.0f,
                                      1.0f / 4.0f,
                                      1.0f / 4.0f,
                                      1.0f / 5.0f,
                                      1.0f / 4.0f,
                                      2.0f / 3.0f,
                                      1.0f / 3.0f,
                                      1.0f / 3.0f,
                                    };

  /*! Shininess for each digit value. */
  float m_anodeShininess[10] = { 10.0f,
                                 20.0f,
                                 10.0f,
                                 10.0f,
                                 10.0f,
                                 10.0f,
                                 10.0f,
                                 10.0f,
                                 10.0f,
                                 10.0f,
                               };

  float m_lightAttenuation = 1.0f; //!< Light attenuation coefficient.
  float m_anodeBrightness[3] = { 0.5f, 1.0f, 2.0f }; //!< To limit the digit brightness when a text is superposed.

  QTimer m_redrawTimer; //!< Redraw timer.
  QTimer m_diaporamaTimer;  //!< Background texture timer.
  QTimer m_motionTimer; //!< Motion timer.

  QImage* m_diaporamaTexture = nullptr; //!< New background texture.
  bool m_separator = true; //!< Separator off or on.
  bool m_forceBackgroundColor = false;  //!< Force background uniform color.
  bool m_dynamicRotation = true; //!< Enable or disable dynamic rotation.

  int m_frameCount = 0; //!< Current frame number.
  QTime m_frameTime; //!< Frame counter timer. Stop after 1s.

  /*! Show or hide a part. */
  quint32 m_partCode = PCBulb       |
                       PCAnode      |
                       PCBase       |
                       PCDigits     |
                       PCFixAnode   |
                       PCFixDigits  |
                       PCTop        |
                       PCCase       |
                       PCRubberFoot |
                       PCTag        |
                       PCFixTop     |
                       PCFixTag     |
                       PCTable      |
                       PCAbout      |
                       PCPencil     |
                       PCSepBulb    |
                       PCSphere     |
                       PCSepBase;
};

void CNixie::drawArrays (EMesh mesh, GLenum mode)
{
  drawArrays (mesh, 0, m_cVertices[mesh], mode);
}

void CNixie::drawArrays (EMesh mesh, int start, int count, GLenum mode)
{
  if (count > 0 && m_validVbos[mesh] && m_vbos[mesh].bind ())
  {
    setupVertexAttribs ();
    glDrawArrays (mode, start, count);
    m_vbos[mesh].release ();
  }
}

void CNixie::updateMatrix ()
{
  m_nixieProgram->setUniformValue (m_uVLocs.m_mvMatrix, m_camera * m_world);
  QMatrix3x3 normalMatrix = m_world.normalMatrix();
  m_nixieProgram->setUniformValue (m_uVLocs.m_normalMatrix, normalMatrix);
}

void CNixie::updateMatrixAndDraw (EMesh mesh, GLenum mode)
{
  updateMatrix ();
  drawArrays (mesh, mode);
}

void CNixie::updateMatrixAndDraw (EMesh mesh, float dx, float dy, float dz, GLenum mode)
{
  m_world.translate (dx, dy, dz);
  updateMatrix ();
  drawArrays (mesh, mode);
}

#endif // NIXIE_HPP
