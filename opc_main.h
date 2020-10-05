class myClassFactory: public IClassFactory	
{
public:
  LONG RefCount;			// reference counter
  LONG server_count;		// server counter
  CRITICAL_SECTION lk;		// protect RefCount 
  myClassFactory(): RefCount(0), server_count(0)	// when creating interface zero all counter
  {  InitializeCriticalSection(&lk);	  }
  ~myClassFactory()
  {  DeleteCriticalSection(&lk); }
// IUnknown realisation
  STDMETHODIMP QueryInterface(REFIID, LPVOID*);
  STDMETHODIMP_(ULONG) AddRef( void);
  STDMETHODIMP_(ULONG) Release( void);
// IClassFactory realisation
  STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID*);
  STDMETHODIMP LockServer(BOOL);
  inline LONG getRefCount(void)
  {
    LONG rc;
    EnterCriticalSection(&lk);		// attempt recieve variable whom may be used by another threads
    rc = RefCount;					// rc = client counter
    LeaveCriticalSection(&lk);                                    
    return rc;
  }
  inline int in_use(void)
  {
    int rv;
    //EnterCriticalSection(&lk);
    rv = 1 | RefCount | server_count;
    //LeaveCriticalSection(&lk);
    return rv;
  }
  inline void serverAdd(void)
  {
    InterlockedIncrement(&server_count);	// increment server counter
  }
  inline void serverRemove(void)
  {
    InterlockedDecrement(&server_count);	// decrement server counter
  }
};
//----- IUnknown -------------------------------------------------------------------------
STDMETHODIMP myClassFactory::QueryInterface(REFIID iid, LPVOID* ppInterface)
{
  if (ppInterface == NULL) return E_INVALIDARG;	// pointer to interface missed (NULL)

  if (iid == IID_IUnknown || iid == IID_IClassFactory)	// legal IID
    {
      UL_DEBUG((LOGID, "myClassFactory::QueryInterface() Ok"));	// write to log
      *ppInterface = this;		// interface succesfully returned
      AddRef();					// adding reference to interface
      return S_OK;				// return succesfully
    }
  UL_ERROR((LOGID, "myClassFactory::QueryInterface() Failed"));
  *ppInterface = NULL;			// no interface returned
  return E_NOINTERFACE;			// error = No Interface
}
STDMETHODIMP_(ULONG) myClassFactory::AddRef(void)	// new client was connected
{
  ULONG rv;
  UL_INFO((LOGID, "AddRef(lk)"));
  EnterCriticalSection(&lk);
  rv = (ULONG)++RefCount;							// increment counter of client
  LeaveCriticalSection(&lk);                                    
  UL_DEBUG((LOGID, "myClassFactory::AddRef(%ld)", rv));	// write to log number
  return rv;
}
STDMETHODIMP_(ULONG) myClassFactory::Release(void)	// client has been disconnected
{
  ULONG rv;
  EnterCriticalSection(&lk);
  rv = (ULONG)--RefCount;							// decrement client counter
  LeaveCriticalSection(&lk);
  UL_DEBUG((LOGID, "myClassFactory::Release(%d)", rv));	// write to log number
  return rv;
}
//----- IClassFactory ----------------------------------------------------------------------
STDMETHODIMP myClassFactory::LockServer(BOOL fLock)
{
  if (fLock)	AddRef();
  else		    Release();
  UL_DEBUG((LOGID, "myClassFactory::LockServer(%d)", fLock)); 
  return S_OK;
}
STDMETHODIMP myClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObject)
{
  if (pUnkOuter != NULL)
    return CLASS_E_NOAGGREGATION; // Aggregation is not supported by this code

  IUnknown *server = 0;
  AddRef(); // for a_server_finished()
  if (loClientCreate(my_service, (loClient**)&server, 0, &vendor, a_server_finished, this))
    {
      UL_DEBUG((LOGID, "myClassFactory::loCreateClient() failed"));
      Release();
      return E_OUTOFMEMORY;	
    }
  serverAdd();
  HRESULT hr = server->QueryInterface(riid, ppvObject);
  if (FAILED(hr))
      UL_DEBUG((LOGID, "myClassFactory::loClient QueryInterface() failed"));
  else
    {
      loSetState(my_service, (loClient*)server, loOP_OPERATE, OPCstatus, 0);
      UL_DEBUG((LOGID, "myClassFactory::server_count = %ld", server_count));
    }
  
  server->Release();
  return hr;
}
static myClassFactory my_CF;
static void a_server_finished(void *a, loService *b, loClient *c)
{
  my_CF.serverRemove();						
  if (a) ((myClassFactory*)a)->Release();
  UL_DEBUG((LOGID, "a_server_finished(%lu)", my_CF.server_count));
}
