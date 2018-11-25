#include "nixie.hpp"
#include "../vmode/stopwatch.hpp"
#include "../vmode/clock.hpp"
#include "../vmode/fastcounter.hpp"
#include "../vmode/thermometer.hpp"
#include "../vmode/timer.hpp"
#include "../vmode/digittest.hpp"
#include <QMouseEvent>
#include <QOpenGLTexture>
#include <QDir>
#include <QThreadPool>
#include <cmath>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_NORMAL_ATTRIBUTE 1
//#define PROGRAM_TEXCOORD_ATTRIBUTE 2

template <typename T>
void setLightUniformValue (QOpenGLShaderProgram* program, char const * property, int index, T const & value)
{
  QByteArray uniformName = "lights[" + QByteArray::number (index) + "]." + property;
  program->setUniformValue (uniformName.constData (), value);
}

CNixie::CNixie (QWidget* parent) : QOpenGLWidget (parent)
{
  m_cVertices.resize (MLastMesh);
  m_vbos.resize (MLastMesh);
  m_validVbos.resize (MLastMesh);
  m_nixiePos.resize (7);
  m_digits.resize (MAXDIGITS);
  m_textures.resize (TLast);
  connect (&m_redrawTimer, &QTimer::timeout, this, &CNixie::updateTubes);
  connect (&m_motionTimer, SIGNAL (timeout()), this, SLOT(update()));
  setMotionInterval (0);
}

CNixie::~CNixie ()
{
  cleanup ();
}

void CNixie::setDisplay (EDisplay display)
{
  if (m_display != display)
  {
    if (display >= LastDisplay)
    {
      display = Nixie;
    }

    m_display = display;
    if (display == Nixie)
    {
      changeVbo ();
      update ();
    }
    else
    {
      for (int i = 0; i < MLastMesh; ++i)
      {
        if (m_vbos[i].isCreated () && i != MBackground)
        {
          m_vbos[i].allocate (0);
          m_validVbos[i] = false;
        }
      }
    }
  }
}

void CNixie::initializeDefaultLightings ()
{
  QVector3D const primaryColorM050 = m_primaryColor * 0.5f;
  QVector3D const primaryColorM020 = m_primaryColor * 0.2f;
  QVector3D const primaryColorM004 = m_primaryColor * 0.04f;
  QVector3D const primaryColorM002 = m_primaryColor * 0.02f;

  m_defaultLightings.resize (PLastPart);
  m_defaultLightings[PBulb].m_light        = m_moodLightIntensities;
  m_defaultLightings[PBulb].m_specular     = QVector3D (1.0f, 1.0f, 1.0f);
  m_defaultLightings[PBulb].m_surfaceColor = primaryColorM004;
  m_defaultLightings[PBulb].m_shininess    = 27.0f;
  m_defaultLightings[PBulb].m_alpha        = 0.3f;

  m_defaultLightings[PAnode].m_light        = primaryColorM020;
  m_defaultLightings[PAnode].m_specular     = primaryColorM020;
  m_defaultLightings[PAnode].m_surfaceColor = primaryColorM020;
  m_defaultLightings[PAnode].m_shininess    = 8.0f;
  m_defaultLightings[PAnode].m_alpha        = 1.0f;

  m_defaultLightings[PBase].m_light        = QVector3D (0.75f, 0.75f, 0.75f);
  m_defaultLightings[PBase].m_specular     = QVector3D (2.0f, 2.0f, 2.0f);
  m_defaultLightings[PBase].m_surfaceColor = QVector3D (0.50754f, 0.50754f, 0.50754f);
  m_defaultLightings[PBase].m_shininess    = 20.0f;
  m_defaultLightings[PBase].m_alpha        = 1.0f;

  m_defaultLightings[PDigits].m_light        = m_primaryColor;
  m_defaultLightings[PDigits].m_specular     = QVector3D (1.0f, 1.0f, 1.0f);
  m_defaultLightings[PDigits].m_surfaceColor = QVector3D (1.0f, 0.1f, 0.02f);
  m_defaultLightings[PDigits].m_shininess    = 1.2f;
  m_defaultLightings[PDigits].m_alpha        = 1.0f;

  m_defaultLightings[PFixAnode].m_light        = primaryColorM050;
  m_defaultLightings[PFixAnode].m_specular     = primaryColorM050;
  m_defaultLightings[PFixAnode].m_surfaceColor = primaryColorM020;
  m_defaultLightings[PFixAnode].m_shininess    = 8.0f;
  m_defaultLightings[PFixAnode].m_alpha        = 1.0f;

  m_defaultLightings[PFixDigits].m_light        = QVector3D (m_primaryColor);
  m_defaultLightings[PFixDigits].m_specular     = primaryColorM002;
  m_defaultLightings[PFixDigits].m_surfaceColor = primaryColorM020;
  m_defaultLightings[PFixDigits].m_shininess    = 1.0f;
  m_defaultLightings[PFixDigits].m_alpha        = 1.0f;

  m_defaultLightings[PTop].m_light        = QVector3D (0.04f, 0.04f, 0.04f);
  m_defaultLightings[PTop].m_specular     = QVector3D (2.0f, 2.0f, 2.0f);
  m_defaultLightings[PTop].m_surfaceColor = QVector3D (0.02f, 0.02f, 0.02f);
  m_defaultLightings[PTop].m_shininess    = 20.0f;
  m_defaultLightings[PTop].m_alpha        = 1.0f;

  m_defaultLightings[PCase].m_light        = QVector3D (0.04f, 0.04f, 0.04f);
  m_defaultLightings[PCase].m_specular     = QVector3D (1.0f, 1.0f, 1.0f);
  m_defaultLightings[PCase].m_surfaceColor = QVector3D (0.50754f, 0.50754f, 0.50754f);
  m_defaultLightings[PCase].m_shininess    = 80.0f;
  m_defaultLightings[PCase].m_alpha        = 1.0f;

  m_defaultLightings[PTag].m_light        = QVector3D (0.75f, 0.75f, 0.75f);
  m_defaultLightings[PTag].m_specular     = QVector3D (2.0f, 2.0f, 2.0f);
  m_defaultLightings[PTag].m_surfaceColor = QVector3D (0.11f, 0.11f, 0.11f);
  m_defaultLightings[PTag].m_shininess    = 26.0f;
  m_defaultLightings[PTag].m_alpha        = 1.0f;

  m_defaultLightings[PPencil].m_light        = QVector3D (1.0f, 1.0f, 1.0f);
  m_defaultLightings[PPencil].m_specular     = QVector3D (0.5f, 0.5f, 0.5f);
  m_defaultLightings[PPencil].m_surfaceColor = QVector3D (0.02f, 0.4f, 0.1f);
  m_defaultLightings[PPencil].m_shininess    = 20.0f;
  m_defaultLightings[PPencil].m_alpha        = 1.0f;

  m_defaultLightings[PPencil2].m_light        = QVector3D (0.5f, 0.5f, 0.5f);
  m_defaultLightings[PPencil2].m_specular     = QVector3D (0.0f, 0.0f, 0.0f);
  m_defaultLightings[PPencil2].m_surfaceColor = QVector3D (0.02f, 0.1f, 0.0f);
  m_defaultLightings[PPencil2].m_shininess    = 1.0f;
  m_defaultLightings[PPencil2].m_alpha        = 1.0f;

  m_defaultLightings[PPencil3].m_light        = QVector3D (0.5f, 0.5f, 0.5f);
  m_defaultLightings[PPencil3].m_specular     = QVector3D (0.2f, 0.2f, 0.2f);
  m_defaultLightings[PPencil3].m_surfaceColor = QVector3D (0.0f, 0.0f, 0.0f);
  m_defaultLightings[PPencil3].m_shininess    = 10.0f;
  m_defaultLightings[PPencil3].m_alpha        = 1.0f;
  m_lightings                                 = m_defaultLightings;
}

void CNixie::setPrimaryColor (QColor const & color)
{
  m_primaryColor.setX (color.red () / 255.0f);
  m_primaryColor.setY (color.green () / 255.0f);
  m_primaryColor.setZ (color.blue () / 255.0f);
}

void CNixie::setType (CMode::EType type, QVariant const & data)
{
  if (m_mode != nullptr)
  {
    CMode::EType currentType = m_mode->type ();
    if (currentType != type)
    {
      if (!m_mode->isExternal ())
      {
        delete m_mode;
      }

      m_mode = nullptr;
    }
    else if (m_mode->isExternal () && data.value<CTimer*> () != m_mode)
    {
      m_mode = nullptr;
    }
  }

  if (m_mode == nullptr)
  {
    m_separator = true;
    switch (type)
    {
      case CMode::Stopwatch :
        m_redrawTimer.stop ();
        m_mode = new CStopwatch ();
        break;

      case CMode::Clock :
        m_redrawTimer.start ();
        m_mode = new CClock (data.toBool ());
        break;

      case CMode::FastCounter :
        m_mode = new CFastCounter ();
        m_redrawTimer.start ();
        break;

      case CMode::DigitTest :
        m_mode = new CDigitTest ();
        m_redrawTimer.start ();
        break;

      case CMode::Thermometer :
        m_mode = new CThermometer (data.toBool ());
        m_redrawTimer.start ();
        break;

      case CMode::Timer :
        m_redrawTimer.start ();
        m_mode = data.value<CTimer*> ();
        break;

      default :
        break;
    }
  }

  if (m_mode != nullptr)
  {
    int interval = m_mode->interval ();
    m_redrawTimer.setInterval (interval);
    if (type != CMode::Thermometer)
    {
      updateTubes ();
    }
  }
}

void CNixie::cleanup ()
{
  m_diaporamaTimer.stop ();
  m_redrawTimer.stop ();

  QThreadPool::globalInstance ()->waitForDone ();

  makeCurrent ();
  for (int k = 0; k < MLastMesh; ++k)
  {
     m_vbos[k].destroy ();
     m_validVbos[k] = false;
  }

  for (QOpenGLTexture* texture : m_textures)
  {
    delete texture;
  }

  m_textures.fill (nullptr);
  delete m_nixieProgram;
  m_nixieProgram = nullptr;
  delete m_diaporamaTexture;
  m_diaporamaTexture = nullptr;
  doneCurrent ();
  if (m_mode != nullptr && !m_mode->isExternal ())
  {
    delete m_mode;
    m_mode = nullptr;
  }
}

void CNixie::updateTubes ()
{
  if (m_defaultLightings.isEmpty ())
  {
    initializeDefaultLightings ();
  }

  int value = m_mode->digits ();
  m_minus   = value < 0;
  value     = std::abs (value);
  for (int i = 0; i < MAXDIGITS; ++i)
  {
    m_digits[i] = value % 10;
    value      /= 10;
  }

  CMode::EType type = m_mode->type ();
  if (m_cNixieTubes == 4 && type == CMode::FastCounter)
  {
    m_digits[0] = m_digits[2];
    m_digits[1] = m_digits[3];
    m_digits[2] = m_digits[4];
    m_digits[3] = m_digits[5];
  }

  repaint ();
  if (type == CMode::Clock || type == CMode::DigitTest)
  {
    m_separator = !m_separator;
  }

  emit clockChanged ();
}

void CNixie::updateBackground ()
{
  if (m_diaporamaTexture == nullptr && m_diaporamaMutex.tryLock ())
  {
    m_diaporamaTexture = new QImage;
    auto diaporama     = new CDiaporama (m_diaporamaIndex, m_diaporamaMutex,
                                            *m_diaporamaTexture, m_diaporamaImages, this,
                                             m_diaporamaMode, m_lightAttenuation);
    m_diaporamaMutex.unlock ();
    QThreadPool::globalInstance ()->start (diaporama);
  }
}

void CNixie::setBackgroundUniformColor (QColor const & color)
{
  m_bkgndTexture.clear ();
  m_bkgndUniformColor = color;
}

void CNixie::setNixieTubeNumber (int count)
{
  m_cNixieTubes      = count;
  float angles[]     = { 0.0f, 60.0f, 50.0f, 35.0f, 23.0f, 30.0f, 23.0f };
  m_fieldOfViewAngle = angles[m_cNixieTubes];

  m_maxCounter = 10;
  for (int i = 1; i < m_cNixieTubes; ++i)
  {
    m_maxCounter *= 10;
  }
}

void CNixie::allocateVbo ()
{
  struct SStl
  {
    QString m_file;
    EPartCode m_part;
  };

  SStl stls[] = { { "0", PCDigits },
                  { "1", PCDigits },
                  { "2", PCDigits },
                  { "3", PCDigits },
                  { "4", PCDigits },
                  { "5", PCDigits },
                  { "6", PCDigits },
                  { "7", PCDigits },
                  { "8", PCDigits },
                  { "9", PCDigits },
                  { "0t", PCAnode },
                  { "1t", PCAnode },
                  { "2t", PCAnode },
                  { "3t", PCAnode },
                  { "4t", PCAnode },
                  { "5t", PCAnode },
                  { "6t", PCAnode },
                  { "7t", PCAnode },
                  { "8t", PCAnode },
                  { "9t", PCAnode },
                  { QString (), PCLast },
                  { "bulb",  PCBulb },
                  { "anode",  PCAnode },
                  { "base",  PCBase },
                  { "fixanode",  PCFixAnode },
                  { "pins",  PCFixAnode },
                  { "fixdigits",  PCFixDigits },
                  { QString ("top%1").arg (m_cNixieTubes), PCTop },
                  { QString ("case%1").arg (m_cNixieTubes), PCCase },
                  { m_cNixieTubes == 3 ? "tag3" : "tag",  PCTag },
                  { "fixtop",  PCFixTop },
                  { "fixtag",  PCFixTag },
                  { "sepbulb",  PCSepBulb },
                  { "sphere",  PCSphere },
                  { "sepbase",  PCBase },
                  { "sepwires",  PCFixAnode },
                  { "minusbulb",  PCBulb },
                  { "minus",  PCDigits },
                  { "minuswires",  PCFixAnode },
                  { "minusbase",  PCBase },
                  { QString (), PCAbout },
                  { "rubberfoot",  PCRubberFoot },
                  { "table",  PCTable },
                  { "pencil",  PCPencil },
                  { "pencil2",  PCPencil },
                  { "pencil3",  PCPencil },
                };

  int cVertices  = 0;
  int cTriangles = 0;
  QDir stlDir (m_stlDirName);

  // Read bulb first to set m_reducingCoefficient
  m_reducingCoefficient = 0.0f;
  int              i    = MBulb;
  QVector<GLfloat> mesh;
  if (m_display == Nixie)
  {
    mesh = readMesh (stlDir.absoluteFilePath (stls[i].m_file));
  }

  if (!mesh.isEmpty ())
  {
    m_cVertices[i] = mesh.size () / 6;
    auto size      = static_cast<int>(byteSize (m_cVertices[i]));
    m_vbos[i].bind ();
    m_vbos[i].allocate (size);
    m_vbos[i].write (0, mesh.constData (), size);
    m_vbos[i].release ();
    m_validVbos[i] = true;
    cVertices     += m_cVertices[i];
    cTriangles    += m_cVertices[i] == 4 ? 2 : m_cVertices[i] / 3;
    mesh.clear ();
  }

  // Read other stl.
  for (i = 0; i < MLastMesh; ++i)
  {
    if (i != MBulb)
    {
      if (i == MBackground)
      {
        mesh = rectangle ();
      }
      else if (i == MAboutRect)
      {
        if (m_display == Nixie)
        {
          mesh = aboutRectangle ();
        }
      }
      else if (m_cNixieTubes == 3 || (m_cNixieTubes != 3 && i != MMinusBulb && i != MMinus && i != MMinusWires && i != MMinusBase))
      {
        if (m_display == Nixie)
        {
          mesh = readMesh (stlDir.absoluteFilePath (stls[i].m_file));
        }
      }

      if (!mesh.isEmpty ())
      {
        m_cVertices[i] = mesh.size () / 6;
        auto size      = static_cast<int>(byteSize (m_cVertices[i]));
        m_vbos[i].bind ();
        m_vbos[i].allocate (size);
        m_vbos[i].write (0, mesh.constData (), size);
        m_vbos[i].release ();
        m_validVbos[i] = true;
        cVertices     += m_cVertices[i];
        cTriangles    += m_cVertices[i] == 4 ? 2 : m_cVertices[i] / 3;
        mesh.clear ();
      }
    }
  }

  qDebug () << "Vertices:" << cVertices << "Triangles:" << cTriangles;
}

void CNixie::changeVbo ()
{
  allocateVbo ();
  resizeGL (width (), height ()); // New projection matrix.
}

void CNixie::initializeGL ()
{
  if (m_mode == nullptr)
  {
    setType (CMode::Clock);
  }

//  connect (context (), &QOpenGLContext::aboutToBeDestroyed, this, &CNixie::cleanup);

  initializeOpenGLFunctions ();
  glClearColor (static_cast<float>(m_bkgndUniformColor.redF ()), static_cast<float>(m_bkgndUniformColor.greenF ()), static_cast<float>(m_bkgndUniformColor.blueF ()), 1.0f);
  setTexture (TBackground, m_bkgndTexture);
  setTexture (TAbout, m_aboutTexture);
  setTexture (TTable, m_tableTexture);

  QString shader = ":/config/bkgnd";
  QString frag   = shader + ".frag", vert = shader + ".vert";
  m_bkgndProgram = new QOpenGLShaderProgram;
  m_bkgndProgram->addShaderFromSourceFile (QOpenGLShader::Vertex, vert);
  m_bkgndProgram->addShaderFromSourceFile (QOpenGLShader::Fragment, frag);
  m_bkgndProgram->bindAttributeLocation ("vertex", PROGRAM_VERTEX_ATTRIBUTE);
  //  m_bkgndProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
  m_bkgndProgram->link ();

  m_bkgndProgram->bind ();
  m_bkgndProgram->setUniformValue ("texture", 0);
  m_bkgndProgram->setUniformValue ("bkgnd_uniform", m_bkgndTexture.isEmpty ());
  m_bkgndProgram->setUniformValue ("bkgnd_color", m_bkgndUniformColor);
  shader = ":/config/bulb";
  frag   = shader + ".frag";
  vert   = shader + ".vert";
  m_nixieProgram = new QOpenGLShaderProgram;
  m_nixieProgram->addShaderFromSourceFile (QOpenGLShader::Vertex, vert);
  m_nixieProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, frag);
  m_nixieProgram->bindAttributeLocation ("vertex", PROGRAM_VERTEX_ATTRIBUTE);
  m_nixieProgram->bindAttributeLocation ("normal", PROGRAM_NORMAL_ATTRIBUTE);
  m_nixieProgram->link ();

  m_nixieProgram->bind ();
  m_uVLocs.m_projMatrix    = m_nixieProgram->uniformLocation ("projMatrix");
  m_uVLocs.m_mvMatrix      = m_nixieProgram->uniformLocation ("mvMatrix");
  m_uVLocs.m_normalMatrix  = m_nixieProgram->uniformLocation ("normalMatrix");
  m_uVLocs.m_cameraPos     = m_nixieProgram->uniformLocation ("cameraPos");
  m_uVLocs.m_surfaceColor  = m_nixieProgram->uniformLocation ("surfaceColor");
  m_uVLocs.m_specularColor = m_nixieProgram->uniformLocation ("specularColor");
  m_uVLocs.m_shininess     = m_nixieProgram->uniformLocation ("shininess");
  m_uVLocs.m_numLights     = m_nixieProgram->uniformLocation ("numLights");
  m_uVLocs.m_digit         = m_nixieProgram->uniformLocation ("digit");
  m_uVLocs.m_texActivated  = m_nixieProgram->uniformLocation ("texActivated");

  // Setup our vertex buffer objects.
  for (int k = 0; k < MLastMesh; ++k)
  {
    m_vbos[k].create ();
  }

  allocateVbo ();

  // Our camera never changes.
  m_nixieProgram->setUniformValue (m_uVLocs.m_cameraPos, QVector3D(0, 0, 100));
  m_camera.setToIdentity ();
  m_camera.translate (0, 0, -1);
  m_nixieProgram->release ();
  m_redrawTimer.start ();
}

void CNixie::paintGL ()
{
  if (m_mode->type () == CMode::FastCounter)
  {
    static_cast<CFastCounter*>(m_mode)->increment ();
    if (m_frameTime.isNull ())
    {
      m_frameTime.start ();
      m_frameCount = 1;
    }
    else
    {
      int ms = m_frameTime.elapsed ();
      if (ms >= 1000)
      {
        float count = m_frameCount / (0.001f * ms);
        emit frameCount (count);
        m_frameCount = 1;
        m_frameTime.restart ();
      }
      else
      {
        ++ m_frameCount;
      }
    }
  }

  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);
  // Diaporama if new image.
  if (!m_forceBackgroundColor)
  {
    if (m_diaporamaTexture != nullptr && !m_diaporamaTexture->isNull () && m_diaporamaMutex.tryLock ())
    {
      if (m_textures[TBackground] != nullptr)
      {
        delete m_textures[TBackground];
      }

      m_textures[TBackground] = new QOpenGLTexture (*m_diaporamaTexture);
      m_textures[TBackground]->setMinificationFilter (QOpenGLTexture::LinearMipMapLinear);
      m_textures[TBackground]->setMagnificationFilter (QOpenGLTexture::Linear);
      delete m_diaporamaTexture;
      m_diaporamaTexture = nullptr;
      m_diaporamaMutex.unlock ();
    }

    // Draw background
    m_bkgndProgram->bind ();
    if (m_textures[TBackground] != nullptr)
    {
      m_bkgndProgram->setUniformValue ("bkgnd_uniform", false);
      m_textures[TBackground]->bind ();
    }

    drawArrays (MBackground, GL_TRIANGLE_STRIP);
    if (m_textures[TBackground] != nullptr)
    {
      m_textures[TBackground]->release ();
    }

    m_bkgndProgram->release ();
  }
  else
  {
    m_bkgndProgram->bind ();
    m_bkgndProgram->setUniformValue ("bkgnd_uniform", true);
    drawArrays (MBackground, GL_TRIANGLE_STRIP);
    m_bkgndProgram->release ();
  }

  if (m_move)
  {
    float c = m_motionTrans.x ();
    if (c <= m_motionMin.x () || c >= m_motionMax.x ())
    {
      m_motionIncs.setX (-m_motionIncs.x ());
    }

    c = m_motionTrans.y ();
    if (c <= m_motionMin.y () || c >= m_motionMax.y ())
    {
      m_motionIncs.setY (-m_motionIncs.y ());
    }

    c = m_motionTrans.z ();
    if (c <= m_motionMin.z () || c >= m_motionMax.z ())
    {
      m_motionIncs.setZ (-m_motionIncs.z ());
    }

    m_motionTrans += m_motionIncs;
  }

  switch (m_display)
  {
    case Text :
      paintText ();
      break;

    case LCD :
    case Leds :
      paintLCD ();
      break;

    default :
      m_world.setToIdentity ();
      m_world.rotate (m_xRot / 16.0f, 1, 0, 0);
      m_world.rotate (m_yRot / 16.0f, 0, 1, 0);
      m_world.rotate (m_zRot / 16.0f, 0, 0, 1);
      if (m_move)
      {
        m_world.translate (m_motionTrans);
      }

      paintNixieTubes ();
      break;
  }
}

void CNixie::setupVertexAttribs ()
{
  size_t const offset = 3 * sizeof(GLfloat);

  QOpenGLFunctions *f = QOpenGLContext::currentContext ()->functions();
  f->glEnableVertexAttribArray (PROGRAM_VERTEX_ATTRIBUTE);
  f->glEnableVertexAttribArray (PROGRAM_NORMAL_ATTRIBUTE);
  f->glVertexAttribPointer (PROGRAM_VERTEX_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (GLfloat), nullptr);
  f->glVertexAttribPointer (PROGRAM_NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (GLfloat), reinterpret_cast<void *>(offset));
}

void CNixie::resizeGL (int w, int h)
{
  float ratio = static_cast<GLfloat>(w) / static_cast<GLfloat>(h);
  m_proj.setToIdentity ();
  m_proj.perspective (m_fieldOfViewAngle, ratio, removeDim (0.01f), m_bulbHeight * m_cNixieTubes * 2);
  m_motionIncs.setY (m_motionIncs.x () / ratio);
}

CNixie::ESection CNixie::clickedSection (int x, int y) const
{
  QRect    rc      = rect ();
  int      w       = rc.width ();
  int      h       = rc.height ();
  int      dx      = w / 10;
  int      dy      = h / 10;
  ESection section = Center;
  if (x < dx)
  {
    section = Left;
  }
  else if (x >  w - dx)
  {
    section = Right;
  }
  else if (y < dy)
  {
    section = Top;
  }
  else if (y > h - dy)
  {
    section = Bottom;
  }

  return section;
}

void CNixie::mousePressEvent (QMouseEvent* event)
{
  m_lastPos = event->pos ();
}

void CNixie::mouseReleaseEvent (QMouseEvent* event)
{
  int x = event->x ();
  int y = event->y ();
  if (!m_dynamicRotation || clickedSection (x, y) != Center)
  {
    emit glWidgetClicked (x, y);
  }
}

void CNixie::mouseMoveEvent (QMouseEvent* event)
{
  int x = event->x ();
  int y = event->y ();
  if (m_dynamicRotation && clickedSection (x, y) == Center && m_display == Nixie)
  {
    int dx = x - m_lastPos.x ();
    int dy = y - m_lastPos.y ();
    unsigned b  = event->buttons ();
    if ((b & Qt::LeftButton) != 0)
    {
      setRotation(m_xRot + 8 * dy, m_xRot);
      setRotation(m_yRot + 8 * dx, m_yRot);
    }
    else if ((b & Qt::RightButton) != 0)
    {
      setRotation(m_xRot + 8 * dy, m_xRot);
      setRotation(m_zRot + 8 * dx, m_zRot);
    }

    m_lastPos = event->pos ();
  }
}

static void normalizeAngle (int& angle)
{
  while (angle < 0)
  {
    angle += 360 * 16;
  }

  while (angle > 360 * 16)
  {
    angle -= 360 * 16;
  }
}

void CNixie::setRotation (int angle, int& rot)
{
  normalizeAngle (angle);
  if (angle != rot)
  {
    rot = angle;
    update ();
  }
}

void CNixie::setAnodeBrightness (EAnodeBrightness brightness)
{
  m_lightings[PDigits].m_light        = m_defaultLightings[PDigits].m_light * m_anodeBrightness[brightness];
  m_lightings[PDigits].m_specular     = m_defaultLightings[PDigits].m_specular * m_anodeBrightness[brightness];
  m_lightings[PDigits].m_surfaceColor = m_defaultLightings[PDigits].m_surfaceColor * m_anodeBrightness[brightness];
}

float CNixie::lightAttenuation () const
{
  float a  = 1.0f;
  float l1 = m_defaultLightings[PDigits].m_light.lengthSquared ();
  float l2 = m_lightings[PDigits].m_light.lengthSquared ();
  if (l2 != 0.0f && l1 != 0.0f)
  {
    a = std::sqrt (l1 / l2);
  }

  return a;
}

void CNixie::setLightAttenuation (float attenuation)
{
  m_lightAttenuation = attenuation;
  m_lightings        = m_defaultLightings;
  if (attenuation != 1.0f)
  {
    for (int i = PBulb; i < PLastPart; ++i)
    {
      m_lightings[i].m_light        /= attenuation;
      m_lightings[i].m_specular     /= attenuation;
      m_lightings[i].m_surfaceColor /= attenuation;
    }
  }

  if (!m_diaporamaTimer.isActive ())
  {
    setTexture (TBackground, m_bkgndTexture);
  }

  setTexture (TAbout, m_aboutTexture);
  setTexture (TTable, m_tableTexture);
  update ();
}

void CNixie::setDiaporamaImages (QStringList const & fileNames)
{
  if (!fileNames.isEmpty ())
  {
    m_diaporamaImages = fileNames;
    connect (&m_diaporamaTimer, &QTimer::timeout, this, &CNixie::updateBackground);
    m_diaporamaTimer.setInterval (m_diaporamaInterval);
    m_diaporamaTimer.start ();
  }
}

void CNixie::setDiaporamaInterval (int interval)
{
  m_diaporamaInterval = interval;
  m_diaporamaTimer.setInterval (m_diaporamaInterval);
}

void CNixie::setTexture (ETexture index, QString const & name)
{
  if (!name.isEmpty ())
  {
    if (m_textures[index] != nullptr)
    {
      delete m_textures[index];
    }

    QImage image = QImage (name);
    if (!image.isNull ())
    {
      m_textures[index] = new QOpenGLTexture (image.mirrored (true, true));
      m_textures[index]->setMinificationFilter (QOpenGLTexture::LinearMipMapLinear);
      m_textures[index]->setMagnificationFilter (QOpenGLTexture::Linear);
    }
  }
}

void CNixie::reversePartCode (EPartCode mode)
{
  if ((m_partCode & mode) != 0)
  {
    m_partCode &= ~mode;
  }
  else
  {
    m_partCode |= mode;
  }
}

void CNixie::stopStartDiaporama ()
{
  if (m_diaporamaTimer.isActive ())
  {
    m_diaporamaTimer.stop ();
  }
  else
  {
    m_forceBackgroundColor = false;
    m_diaporamaTimer.start ();
  }
}

bool CNixie::isDiaporamaActive () const
{
  return m_diaporamaTimer.isActive ();
}

void CNixie::forceReleaseBackgroungUniform ()
{
  m_forceBackgroundColor = !m_forceBackgroundColor;
  if (!m_diaporamaImages.isEmpty ())
  {
    if (m_forceBackgroundColor)
    {
      m_diaporamaTimer.stop ();
    }
    else
    {
      m_diaporamaTimer.start ();
    }
  }
}

void CNixie::setMotionMin (QVector3D const & p)
{
  m_motionMin = p;
}

void CNixie::setMotionMax (QVector3D const & p)
{
  m_motionMax = p;
}

void CNixie::setInMotion (bool move)
{
  m_move = move;
  if (move)
  {
    m_motionTimer.start ();
  }
  else
  {
    m_motionTimer.stop ();
  }
}

void CNixie::setMotionIncr (QVector3D const & incs)
{
  m_motionIncs = incs;
}

void CNixie::setMoodLightPositions (float x, float y, float z)
{
  m_iniMoodLightPositions[0] = QVector4D (-x, y, z, 1);
  m_iniMoodLightPositions[1] = QVector4D (x, y, z, 1);
  m_iniMoodLightPositions[2] = QVector4D (0, z, 0, 1);
}

void CNixie::setMoodLightIntensities (float r, float g, float b)
{
  m_moodLightIntensities = QVector3D (r, g, b);
}
