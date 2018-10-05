#include "winbrightness.hpp"
#include <QtDebug>
#include <windows.h>
#include <ntddvdeo.h>

CWinBrightness::CWinBrightness ()
{
  HANDLE hDevice = CreateFileA ( m_fileName,
                                 0,
                                 FILE_SHARE_READ,
                                 nullptr,
                                 OPEN_EXISTING,
                                 0,
                                 nullptr
                                 );

  if (hDevice != INVALID_HANDLE_VALUE)
  {
    unsigned char outBuffer[256];
    DWORD         bytesReturned;
    m_done = DeviceIoControl ( hDevice,
                               IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS,
                               nullptr,
                               0,
                               outBuffer,
                               sizeof (outBuffer),
                               &bytesReturned,
                               nullptr
                               );

    CloseHandle (hDevice);
    m_min = outBuffer[0];
    m_max = outBuffer[bytesReturned - 1];
  }

  if (!m_done)
  {
    qDebug () << "Win error:" << GetLastError ();
  }
}

float CWinBrightness::value () const
{
  bool   done    = false;
  HANDLE hDevice = CreateFileA (m_fileName,
                                0,
                                FILE_SHARE_READ,
                                nullptr,
                                OPEN_EXISTING,
                                0,
                                nullptr
                               );

  int value = 0;
  if (hDevice != INVALID_HANDLE_VALUE)
  {
    DWORD               bytesReturned;
    _DISPLAY_BRIGHTNESS brightness;
    done = DeviceIoControl (hDevice,
                            IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,
                            nullptr,
                            0,
                            &brightness,
                            sizeof (_DISPLAY_BRIGHTNESS),
                            &bytesReturned,
                            nullptr
                           );

    CloseHandle (hDevice);
    value = brightness.ucDCBrightness;
  }

  if (!done)
  {
    qDebug () << "Win error:" << GetLastError ();
  }

  return softwareValue (value);
}

bool CWinBrightness::setValue (float value)
{
  bool   done    = false;
  HANDLE hDevice = CreateFileA (m_fileName,
                                0,
                                FILE_SHARE_WRITE,
                                nullptr,
                                OPEN_EXISTING,
                                0,
                                nullptr
                                );

  if (hDevice != INVALID_HANDLE_VALUE)
  {
    DWORD               bytesReturned;
    _DISPLAY_BRIGHTNESS brightness;
    brightness.ucDisplayPolicy = DISPLAYPOLICY_BOTH;
    brightness.ucACBrightness = brightness.ucDCBrightness = hardwareValue (value);
    done = DeviceIoControl (hDevice,
                            IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS,
                            &brightness,
                            sizeof (_DISPLAY_BRIGHTNESS),
                            nullptr,
                            0,
                            &bytesReturned,
                            nullptr
                           );

    CloseHandle (hDevice);
  }

  if (!done)
  {
    qDebug () << "Win error:" << GetLastError ();
  }

  return done;
}

bool CWinBrightness::connected () const
{
  return m_done;
}


