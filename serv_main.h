VOID timetToFileTime( time_t t, LPFILETIME pft )
{   LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime =  (ULONG)(ll >>32);}
CHAR *absPath(CHAR *fileName)					// return abs path of file
{ static char path[sizeof(argv0)]="\0";			// path - massive of comline
  CHAR *cp;
  if(*path=='\0') strcpy_s(path, sizeof (argv0), argv0);
  if(NULL==(cp=strrchr(path,'\\'))) cp=path; else cp++;
  strcpy_s(cp,100, fileName);
  return path;}

inline void init_common(void)		// create log-file
{ logg = unilog_Create(ECL_SID, LOG_FNAME, NULL, 0, ll_DEBUG); // level [ll_FATAL...ll_DEBUG] 
  UL_INFO((LOGID, "Unknown Device OPC server start"));}

inline void cleanup_common(void)	// delete log-file
{ UL_INFO((LOGID, "Finish"));
  unilog_Delete(logg); logg = NULL;
  UL_INFO((LOGID, "total Finish"));}

INT show_error(LPCTSTR msg)			// just show messagebox with error
{ ::MessageBox(NULL, msg, L"opc.device", MB_ICONSTOP|MB_OK);
  return 1;}
INT show_msg(LPCTSTR msg)			// just show messagebox with message
{ ::MessageBox(NULL, msg, L"opc.device", MB_OK);
  return 1;}

inline void cleanup_all(DWORD objid)
{ // Informs OLE that a class object, previously registered is no longer available for use  
  if (FAILED(CoRevokeClassObject(objid)))  UL_WARNING((LOGID, "CoRevokeClassObject() failed..."));
  DestroyDriver();					// close port and destroy driver
  CoUninitialize();					// Closes the COM library on the current thread
  cleanup_common();					// delete log-file
}
