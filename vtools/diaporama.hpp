#ifndef DIAPORAMA_HPP
#define DIAPORAMA_HPP

#include "vtools_globals.h"
#include <QRunnable>

class QMutex;
class QStringList;
class QWidget;
class QImage;

/*! \brief CDiaporama is the class to manage a diaporam from a list of images.
 */
class VTOOLSSHARED_EXPORT CDiaporama : public QRunnable
{
public:
  /*! Conversion image. */
  enum EMode { FullImage, //!< The images is ajusted from the content. The dimensions are not preserved.
               CropImage, //!< The images is cropped to preserve the dimensions.
             };

  /*! Constructor.
   * \param index: The image index to read in entry and the next image index to read in out. If index is the
   * last image, 0 is returned
   * \param mutex: A mutex used. The image read is executed in another thread.
   * \param texture: The image read.
   * \param images: The image list.
   * \param widget: The widget to define the dimensions.
   * \param mode: The mode choosen.
   * \param attenuation: The attenuation coefficient [0:1].
   */
  CDiaporama (int& index, QMutex& mutex, QImage& texture, QStringList const & images,
              QWidget* widget, EMode mode, float attenuation = 1.0f);

  /*! Returns an attenuated image of image.
   * \param image: The image source.
   * \param attenuation: The attenuation coefficient [0:1].
   * \remark This function is not used actually.
   */
  static QImage applyAttenuation (QImage image, float attenuation);

private :
  /*! The function launched by the thread. */
  virtual void run ();

private :
  int& m_index; //!< The image index to read in entry and the next image index to read in out.
  QMutex& m_mutex; //!< The mutex used to synchronize the threads.
  QImage& m_texture; //!< The image read.
  QStringList const & m_images; //!< The list of images.
  QWidget* m_widget; //!< The widget used to show the images.
  EMode m_mode; //!< The mode.
  float m_attenuation; //!< The attenuation coefficient.
};

#endif // DIAPORAMA_HPP
