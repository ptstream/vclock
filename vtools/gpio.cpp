#include "gpio.hpp"

#ifdef RASBIANGPIO
#include <QFile>

static QString path = "/sys/class/gpio/";

CGpioPin::CGpioPin (int pin) : m_pin (pin)
{
  QFile file (path + "export");
  if (file.open (QIODevice::WriteOnly))
  {
    QByteArray data    = QByteArray::number (pin);
    qint64     written = file.write (data);
    m_done             = written == data.size ();
  }
}

CGpioPin::~CGpioPin ()
{
  QFile file (path + "unexport");
  if (file.open (QIODevice::WriteOnly))
  {
    QByteArray data = QByteArray::number (m_pin);
    file.write (data);
  }
}

QString CGpioPin::filePath (char const * subDir) const
{
  return path + QString ("gpio%1/%2").arg (m_pin).arg (subDir);
}

bool CGpioPin::setDirection (EDirection dir)
{
  bool done = false;
  if (dir != UndefinedDir)
  {
    QFile file (filePath ("direction"));
    if (file.open (QIODevice::WriteOnly))
    {
      QByteArray cdir    = dir == In ? "in\n" : "out\n";
      qint64     written = file.write (cdir);
      done               = written == cdir.size ();
    }
  }

  return done;
}

CGpioPin::EDirection CGpioPin::direction () const
{
  EDirection dir = UndefinedDir;
  QFile      file (filePath ("direction"));
  if (file.open (QIODevice::ReadOnly))
  {
    QByteArray content = file.readAll ();
    if (content.size () >= 2)
    {
      if (content[0] == 'i')
      {
        dir = In;
      }
      else if (content[0] == 'o')
      {
        dir = Out;
      }
    }
  }

  return dir;
}

CGpioPin::EState CGpioPin::state () const
{
  EState st = UndefinedState;
  QFile  file (filePath ("value"));
  if (file.open (QIODevice::ReadOnly))
  {
    QByteArray content = file.readAll ();
    if (content.size () >= 1)
    {
      st = content[0] == '0' ? Off : On;
    }
  }

  return st;
}

CGpio::CGpio ()
{
  m_pins = { 17, 18, 27, 22, 23, 24, 25, 4, 28, 29, 30, 31 };
  m_cPins = static_cast<int>(sizeof (m_pins) / sizeof (int));
}

CGpio::~CGpio ()
{
  for (QMap<int, CGpioPin*>::iterator it = begin (), last = end (); it != last; ++it)
  {
    delete it.value ();
  }
}

CGpioPin* CGpio::eXport (int index)
{
  CGpioPin* pin = nullptr;
  if (!contains (m_pins[index]))
  {
    pin  = new CGpioPin (m_pins[index]);
    if (pin->done ())
    {
      insert (index, pin);
    }
    else
    {
      delete pin;
      pin = nullptr;
    }
  }
  else
  {
    pin = value (index);
  }

  return pin;
}

bool CGpio::unExport (int index)
{
  bool done = false;
  if (contains (index))
  {
    delete value (index);
    done = remove (index) > 0;
  }

  return done;
}

CGpioPin::EDirection CGpio::direction (int index) const
{
  CGpioPin::EDirection dir = CGpioPin::UndefinedDir;
  if (contains (index))
  {
    dir = value (index)->direction ();
  }

  return dir;
}

bool CGpio::setDirection (int index, CGpioPin::EDirection dir)
{
  bool done = false;
  if (contains (index))
  {
    value (index)->setDirection (dir);
    done = true;
  }

  return done;
}

CGpioPin::EState CGpio::state (int index) const
{
  CGpioPin::EState state = CGpioPin::UndefinedState;
  if (contains (index))
  {
    state = value (index)->state ();
  }

  return state;
}

#endif // RASBIANGPIO
