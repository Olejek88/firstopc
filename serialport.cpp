#define _CRT_SECURE_NO_WARNINGS 1 
#include <TCHAR.H >

#include "serialport.h"
#include "winerror.h"
#include <stdio.h>
#include "unilog.h"				// universal utilites for creating log-files

extern int LOGID; 				// log identifiner

class _SERIAL_PORT_DATA
{
public:	//Constructors /Destructors
  _SERIAL_PORT_DATA();
  ~_SERIAL_PORT_DATA();

  HINSTANCE m_hKernel32;
  typedef BOOL (CANCELIO)(HANDLE);
  typedef CANCELIO* LPCANCELIO;
  LPCANCELIO m_lpfnCancelIo;
};

_SERIAL_PORT_DATA::_SERIAL_PORT_DATA()
{
  m_hKernel32 = LoadLibrary(TEXT("KERNEL32.DLL"));
  if (m_hKernel32 != NULL) 
	m_lpfnCancelIo = (LPCANCELIO) GetProcAddress(m_hKernel32, "CancelIo");
}

_SERIAL_PORT_DATA::~_SERIAL_PORT_DATA()
{
  FreeLibrary(m_hKernel32);
  m_hKernel32 = NULL;
}

_SERIAL_PORT_DATA _SerialPortData;

SerialPort::SerialPort()
{
  m_hComm = INVALID_HANDLE_VALUE;
  m_bOverlapped = FALSE;
}

SerialPort::~SerialPort()
{
  Close();
}

BOOL SerialPort::Open(int nPort, DWORD dwBaud, Parity parity, BYTE DataBits, StopBits stopbits, FlowControl fc, BOOL bOverlapped)
{
  //Validate our parameters
  if (!(nPort>0 && nPort<=255)) return FALSE;
  //Call CreateFile to open up the comms port
  CHAR sPort[100];

  sprintf (sPort,"\\\\.\\COM%d", nPort);
  //m_hComm = CreateFile((LPCWSTR)sPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==1) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM1"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==2) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM2"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==3) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM3"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==4) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM4"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==5) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM5"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==6) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM6"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==7) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM7"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==8) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM8"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==9) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM9"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==10) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM10"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==11) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM11"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==12) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM12"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);  
  if (nPort==13) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM13"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==14) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM14"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==15) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM15"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==16) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM16"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==17) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM17"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==18) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM18"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==19) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM19"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (nPort==20) m_hComm = CreateFile((LPCWSTR)_T("\\\\.\\COM20"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);

  if (m_hComm == INVALID_HANDLE_VALUE)
    return FALSE;

  m_bOverlapped = bOverlapped;
  //Get the curregnt state prior to changing it
  DCB dcb;
  GetState(dcb);

  //Setup the baud rate
  dcb.BaudRate = dwBaud; 

  //Setup the Parity
  switch (parity)
  {
    case EvenParity:  dcb.Parity = EVENPARITY;  break;
    case MarkParity:  dcb.Parity = MARKPARITY;  break;
    case NoParity:    dcb.Parity = NOPARITY;    break;
    case OddParity:   dcb.Parity = ODDPARITY;   break;
    case SpaceParity: dcb.Parity = SPACEPARITY; break;
    default:          return FALSE;
  }

  //Setup the data bits
  dcb.ByteSize = DataBits;

  //Setup the stop bits
  switch (stopbits)
  {
    case OneStopBit:           dcb.StopBits = ONESTOPBIT;   break;
    case OnePointFiveStopBits: dcb.StopBits = ONE5STOPBITS; break;
    case TwoStopBits:          dcb.StopBits = TWOSTOPBITS;  break;
    default:                   return FALSE;
  }

  //Setup the flow control 
  dcb.fDsrSensitivity = FALSE;
  switch (fc)
  {
    case NoFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case CtsRtsFlowControl:
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case CtsDtrFlowControl:
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case DsrRtsFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case DsrDtrFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case XonXoffFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fOutX = TRUE;
      dcb.fInX = TRUE;
      dcb.XonChar = 0x11;
      dcb.XoffChar = 0x13;
      dcb.XoffLim = 100;
      dcb.XonLim = 100;
      break;
    }
    default:
    {
      return FALSE;
      break;
    }
  }  
  //Now that we have all the settings in place, make the changes
  SetState(dcb);
  return TRUE;
}

BOOL SerialPort::Close()
{
  if (IsOpen())
  {
    BOOL bSuccess = CloseHandle(m_hComm);
    m_hComm = INVALID_HANDLE_VALUE;
    if (!bSuccess)
      return FALSE;
    m_bOverlapped = FALSE;
  }
  return TRUE;
}

DWORD SerialPort::Read(void* lpBuf, DWORD dwCount)
{
  if (!IsOpen()) return -1;
  if (m_bOverlapped) return -1;

  DWORD dwBytesRead = 0;
  if (!ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, NULL))
    return -1;
  return dwBytesRead;
}

DWORD SerialPort::Write(const void* lpBuf, DWORD dwCount)
{
  if (!IsOpen()) return -1;
  if (m_bOverlapped) return -1;

  DWORD dwBytesWritten = 0;
  if (!WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, NULL))
    return -1;
  return dwBytesWritten;
}

void SerialPort::SetBreak()
{
  if (!IsOpen()) return;
  SetCommBreak(m_hComm);
}

void SerialPort::ClearBreak()
{
  if (!IsOpen()) return;
  ClearCommBreak(m_hComm);
}

void SerialPort::ClearError(DWORD& dwErrors)
{
  if (!IsOpen()) return;
  ClearCommError(m_hComm, &dwErrors, NULL);
}

void SerialPort::GetStatus(COMSTAT& stat)
{
  if (!IsOpen()) return;
  DWORD dwErrors;
  ClearCommError(m_hComm, &dwErrors, &stat);
}

void SerialPort::GetState(DCB& dcb)
{
  if (!IsOpen()) return;
  GetCommState(m_hComm, &dcb);
}

void SerialPort::SetState(DCB& dcb)
{
  if (!IsOpen()) return;
  SetCommState(m_hComm, &dcb);
}

void SerialPort::Escape(DWORD dwFunc)
{
  if (!IsOpen()) return;
  EscapeCommFunction(m_hComm, dwFunc);
}

void SerialPort::ClearDTR()
{
  Escape(CLRDTR);
}

void SerialPort::ClearRTS()
{
  Escape(CLRRTS);
}

void SerialPort::SetDTR()
{
  Escape(SETDTR);
}

void SerialPort::SetRTS()
{
  Escape(SETRTS);
}

void SerialPort::SetXOFF()
{
  Escape(SETXOFF);
}

void SerialPort::SetXON()
{
  Escape(SETXON);
}

void SerialPort::GetProperties(COMMPROP& properties)
{
  if (!IsOpen()) return;
  GetCommProperties(m_hComm, &properties);
}

void SerialPort::GetModemStatus(DWORD& dwModemStatus)
{
  if (!IsOpen()) return;
  GetCommModemStatus(m_hComm, &dwModemStatus);
}

void SerialPort::SetMask(DWORD dwMask)
{
  if (!IsOpen()) return;
  SetCommMask(m_hComm, dwMask);
}

void SerialPort::GetMask(DWORD& dwMask)
{
  if (!IsOpen()) return;
  GetCommMask(m_hComm, &dwMask);
}

void SerialPort::Flush()
{
  if (!IsOpen()) return;
  FlushFileBuffers(m_hComm);
}

void SerialPort::Purge(DWORD dwFlags)
{
  if (!IsOpen()) return;
  PurgeComm(m_hComm, dwFlags);
}

void SerialPort::TerminateOutstandingWrites()
{
  Purge(PURGE_TXABORT);
}

void SerialPort::TerminateOutstandingReads()
{
  Purge(PURGE_RXABORT);
}

void SerialPort::ClearWriteBuffer()
{
  Purge(PURGE_TXCLEAR);
}

void SerialPort::ClearReadBuffer()
{
  Purge(PURGE_RXCLEAR);
}

void SerialPort::Setup(DWORD dwInQueue, DWORD dwOutQueue)
{
  if (!IsOpen()) return;
  SetupComm(m_hComm, dwInQueue, dwOutQueue);
}

void SerialPort::SetTimeouts(COMMTIMEOUTS& timeouts)
{
  if (!IsOpen()) return;
  SetCommTimeouts(m_hComm, &timeouts);
}

void SerialPort::GetTimeouts(COMMTIMEOUTS& timeouts)
{
  if (!IsOpen()) return;
  GetCommTimeouts(m_hComm, &timeouts);
}

void SerialPort::Set0Timeout()
{
  COMMTIMEOUTS Timeouts;
  ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));
  Timeouts.ReadIntervalTimeout = MAXDWORD;
  Timeouts.ReadTotalTimeoutMultiplier = 0;
  Timeouts.ReadTotalTimeoutConstant = 0;
  Timeouts.WriteTotalTimeoutMultiplier = 0;
  Timeouts.WriteTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void SerialPort::Set0WriteTimeout()
{
  COMMTIMEOUTS Timeouts;
  GetTimeouts(Timeouts);
  Timeouts.WriteTotalTimeoutMultiplier = 0;
  Timeouts.WriteTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void SerialPort::Set0ReadTimeout()
{
  COMMTIMEOUTS Timeouts;
  GetTimeouts(Timeouts);
  Timeouts.ReadIntervalTimeout = MAXDWORD;
  Timeouts.ReadTotalTimeoutMultiplier = 0;
  Timeouts.ReadTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void SerialPort::WaitEvent(DWORD& dwMask)
{
  if (!IsOpen()) return;
  if (m_bOverlapped) return;
  WaitCommEvent(m_hComm, &dwMask, NULL);
}