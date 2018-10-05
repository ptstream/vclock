#ifndef GPIO_HPP
#define GPIO_HPP

//#define RASBIANGPIO

#ifdef RASBIANGPIO

#include "vtools_globals.h"
#include <QString>
#include <QMap>
#include "array"

/*! \brief The definition of a gpio pin.
 * The pin index has the same definition that WiringPi library.
 */
class VTOOLSSHARED_EXPORT CGpioPin
{
public:
  /*! Pin direction. */
  enum EDirection {
                    UndefinedDir, // Undefined direction.
                    In, // In direction.
                    Out // Out direction.
                  };

  /*! Pin state. */
  enum EState {
                UndefinedState = -1, // Undefined state.
                Off = 0, // Off state.
                On = 1 // On state.
              };

  /*! Constructor. */
  CGpioPin (int pin);

  /*! Destructor. */
  ~CGpioPin ();

  /*! Returns true if the pin is correctly exported. */
  bool done () const { return m_done; }

  /*! Sets the pin direction. */
  bool setDirection (EDirection dir);

  /*! Returns the actual pin direction. */
  EDirection direction () const;

  /*! Returns the actual pin state. */
  EState state () const;

private :
  /*! Returns the full path of the Raspbian file. */
  QString filePath (char const * subDir) const;

private :
  bool m_done = false; //!< Exported state.
  int m_pin = 0; //!< Pin index.
};

/*! \brief The gpio manager. */
class VTOOLSSHARED_EXPORT CGpio : public QMap<int, CGpioPin*>
{
public :
  /*! Constructor. */
  CGpio ();

  /*! Destructor. */
  ~CGpio ();

  /*! Export a pin.
   * \param index: The pin index (same definition that WiringPi library).
   * \return The corresponding gpio pin or null in case of fail.
   */
  CGpioPin* eXport(int index);

  /*! Unexport a pin.
   * \param index: The pin index (same definition that WiringPi library).
   * \return False in case of fail.
   */
  bool unExport (int index);

  /*! Returns the pin direction.
   * \param index: The pin index.
   * \return The direction or UndefinedDir in case of fail.
   */
  CGpioPin::EDirection direction (int index) const;

  /*! Sets the pin direction.
   * \param index: The pin index.
   * \param dir: The direction.
   * \return False in case of fail.
   */
  bool setDirection (int index, CGpioPin::EDirection dir);

  /*! Returns the pin state.
   * \param index: The pin index.
   )* \return The pin state or UndefinedState in case of fail.
   */
  CGpioPin::EState state (int index) const;

private :
  std::array<int, 12> m_pins; //!< WiringPi numerotation e.g. 0 --> GPIO17
  int m_cPins; //!< The number of pins (12).
};

#endif // RASBIANGPIO

#endif // GPIO_HPP
