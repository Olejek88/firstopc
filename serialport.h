#include <windows.h>

class SerialPort
{
public:
//Enums
  enum FlowControl
  {
    NoFlowControl,
    CtsRtsFlowControl,
    CtsDtrFlowControl,
    DsrRtsFlowControl,
    DsrDtrFlowControl,
    XonXoffFlowControl
  };

  enum Parity
  {    
    EvenParity,
    MarkParity,
    NoParity,
    OddParity,
    SpaceParity
  };

  enum StopBits
  {
    OneStopBit,
    OnePointFiveStopBits,
    TwoStopBits
  };

//Constructors / Destructors
  SerialPort();
  ~SerialPort();

//General Methods
  BOOL Open(int nPort, DWORD dwBaud = 1200, Parity parity = OddParity, BYTE DataBits = 8,StopBits stopbits = OneStopBit, FlowControl fc = NoFlowControl, BOOL bOverlapped = FALSE);
  BOOL Close();
  BOOL IsOpen() const { return m_hComm != INVALID_HANDLE_VALUE; };
  HANDLE ReadHandle() { return m_hComm; }

//Reading / Writing Methods
  DWORD Read(void* lpBuf, DWORD dwCount);
  BOOL Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped);
  DWORD Write(const void* lpBuf, DWORD dwCount);
  BOOL Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped);

//Misc RS232 Methods
  void ClearBreak();
  void SetBreak();
  void ClearError(DWORD& dwErrors);
  void GetStatus(COMSTAT& stat);
  void GetState(DCB& dcb);
  void SetState(DCB& dcb);
  void Escape(DWORD dwFunc);
  void ClearDTR();
  void ClearRTS();
  void SetDTR();
  void SetRTS();
  void SetXOFF();
  void SetXON();
  void GetProperties(COMMPROP& properties);
  void GetModemStatus(DWORD& dwModemStatus);

//Timeouts
  void SetTimeouts(COMMTIMEOUTS& timeouts);
  void GetTimeouts(COMMTIMEOUTS& timeouts);
  void Set0Timeout();
  void Set0WriteTimeout();
  void Set0ReadTimeout();

//Event Methods
  void SetMask(DWORD dwMask);
  void GetMask(DWORD& dwMask);
  void WaitEvent(DWORD& dwMask);
  void WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped);
  
//Queue Methods
  void Flush();
  void Purge(DWORD dwFlags);
  void TerminateOutstandingWrites();
  void TerminateOutstandingReads();
  void ClearWriteBuffer();
  void ClearReadBuffer();
  void Setup(DWORD dwInQueue, DWORD dwOutQueue);

protected:
  HANDLE m_hComm;       //Handle to the comms port
  BOOL   m_bOverlapped; //Is the port open in overlapped IO
};

