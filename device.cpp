#define _CRT_SECURE_NO_WARNINGS 1 
#define _WIN32_DCOM				// Enables DCOM extensions
#define INITGUID				// Initialize OLE constants
#define ECL_SID "opc.device"	// identificator of OPC server

#include <stdio.h>
#include "device.h"				// server variable
#include "unilog.h"				// universal utilites for creating log-files
#include <locale.h>				// set russian codepage
#include "opcda.h"				// basic function for OPC:DA
#include "lightopc.h"			// light OPC library header file
#include <TCHAR.H >
#include <conio.h>
//---------------------------------------------------------------------------------
INT mymain(HINSTANCE hInstance, INT argc, CHAR *argv[]);
UINT DestroyDriver();		
UINT InitDriver();
VOID poll_device();
//---------------------------------------------------------------------------------
static CRITICAL_SECTION lk_values;	// защита ti[] от совместного доступа
static CHAR *tn[TAGS_NUM_MAX];		// имя тега
static loTagValue tv[TAGS_NUM_MAX];	// значение
static loTagId ti[TAGS_NUM_MAX];	// идентификатор
//---------------------------------------------------------------------------------
UINT devNum=2;						// количество устройств
UINT tag_num=6;						// количество тегов
UINT tTotal=0;						// общее количество тегов
//---------------------------------------------------------------------------------
unilog	*logg=NULL;					
loService *my_service;				
static const loVendorInfo vendor = {0,8,8,0,"Unknown device OPC Server" };	// OPC vendor info (Major/Minor/Build/Reserv)
static void a_server_finished(VOID*, loService*, loClient*);				// OnServer finish his work
static int OPCstatus=OPC_STATUS_RUNNING;									// status of OPC server
//---------------------------------------------------------------------------------
WCHAR wargv0[FILENAME_MAX + 32];	// lenght of command line (file+path (260+32))
CHAR argv0[FILENAME_MAX + 32];		// lenght of command line (file+path (260+32))
//---------------------------------------------------------------------------------
// {1F45684E-9A5F-499a-851E-63478C58B84C}
DEFINE_GUID(GID_UnknownDeviceOPCserverExe, 
0x1f45684e, 0x9a5f, 0x499a, 0x85, 0x1e, 0x63, 0x47, 0x8c, 0x58, 0xb8, 0x4c);
//---------------------------------------------------------------------------------
#include "serv_main.h"	//	main server 
#include "opc_main.h"	//	main server 
//---------------------------------------------------------------------------------
INT APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,INT nCmdShow)
{ static char *argv[3] = { "dummy.exe", NULL, NULL };	// defaults arguments
  argv[1] = lpCmdLine;									// comandline - progs keys
  return mymain(hInstance, 2, argv);}
//---------------------------------------------------------------------------------
INT main(INT argc, CHAR *argv[])
{  return mymain(GetModuleHandle(NULL), argc, argv); }
//---------------------------------------------------------------------------------
INT mymain(HINSTANCE hInstance, INT argc, CHAR *argv[]) 
{
 const char eClsidName [] = ECL_SID;
 const char eProgID [] = ECL_SID;
 CHAR *cp;
 DWORD objid=::GetModuleFileName(NULL, wargv0, sizeof(wargv0));
 WideCharToMultiByte(CP_ACP,0,wargv0,-1,argv0,300,NULL, NULL);
// if(objid==0 || objid+50 > sizeof(argv0)) return 0;
 init_common();									// create log-file
 UL_ERROR((LOGID, "system path [%s]",argv0));	// in bad case write error in log
 
 /*if(NULL==(cp = setlocale(LC_ALL, ".1251")))		// sets all categories, returning only the string cp-1251
	{ 
	 UL_ERROR((LOGID, "setlocale() - Can't set 1251 code page"));	// in bad case write error in log
	 cleanup_common();							// delete log-file
     return 0;
	}*/

 cp = argv[1];		
 if(cp)						// check keys of command line 
	{
    INT finish = 1;			// flag of comlection
    if (strstr(cp, "/r"))	//	attempt registred server
		{
	     if (loServerRegister(&GID_UnknownDeviceOPCserverExe, eProgID, eClsidName, argv0, 0)) 
			{ show_error(L"Registration Failed");  UL_ERROR((LOGID, "Registration <%s> <%s> Failed", eProgID, argv0));  } 
		 else 
			{ show_msg(L"Unknown device OPC Registration Ok"); UL_INFO((LOGID, "Registration <%s> <%s> Ok", eProgID, argv0));  }
		} 
	else 
		if (strstr(cp, "/u")) 
			{
			 if (loServerUnregister(&GID_UnknownDeviceOPCserverExe, eClsidName)) 
				{ show_error(L"UnRegistration Failed"); UL_ERROR((LOGID, "UnReg <%s> <%s> Failed", eClsidName, argv0)); } 
			 else 
				{ show_msg(L"Unknown device OPC Server Unregistered"); UL_INFO((LOGID, "UnReg <%s> <%s> Ok", eClsidName, argv0)); }
			} 
		//if (finish) {      cleanup_common();      return 0;    } 
	}
 if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) 
	{	// Initializes the COM library for use by the calling thread
     UL_ERROR((LOGID, "CoInitializeEx() failed. Exiting..."));
     cleanup_common();	// close log-file
     return 0;
	}
 UL_INFO((LOGID, "CoInitializeEx() [ok]"));	// write to log

 if (InitDriver()) {		// open sockets and attempt connect to servers
    CoUninitialize();	// Closes the COM library on the current thread
    cleanup_common();	// close log-file
    return 0;
  }
 UL_INFO((LOGID, "InitDriver() [ok]"));	// write to log

 if (FAILED(CoRegisterClassObject(GID_UnknownDeviceOPCserverExe, &my_CF, 
				   CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER|CLSCTX_INPROC_SERVER, 
				   REGCLS_MULTIPLEUSE, &objid)))
    { UL_ERROR((LOGID, "CoRegisterClassObject() [failed]"));
      cleanup_all(objid);		// close socket and unload all librares
      return 0; }
 UL_INFO((LOGID, "CoRegisterClassObject() [ok]"));	// write to log
 Sleep(1000);
 my_CF.Release();

 while(my_CF.in_use())	// while server created or client connected
	{
	 poll_device();		// polling devices else do nothing (and be nothing)	 
	}
 UL_INFO((LOGID, "end cleanup_all()"));	// write to log
 cleanup_all(objid);		// destroy himself
 return 0;
}
//----------------------------------------------------------------------------------------------
VOID activation_monitor(const loCaller *ca, INT count, loTagPair *til){}
//----------------------------------------------------------------------------------------------
loTrid ReadTags(const loCaller *, unsigned  count, loTagPair taglist[],
		VARIANT   values[],	WORD      qualities[],	FILETIME  stamps[],
		HRESULT   errs[],	HRESULT  *master_err,	HRESULT  *master_qual,
		const VARTYPE vtype[],	LCID lcid)
{  return loDR_STORED; }
//----------------------------------------------------------------------------------------------
INT WriteTags(const loCaller *ca,
              unsigned count, loTagPair taglist[],
              VARIANT values[], HRESULT error[], HRESULT *master, LCID lcid)
{   return loDW_TOCACHE; }
//----------------------------------------------------------------------------------------------
UINT InitDriver()
{
 UL_INFO((LOGID, "InitDriver() start"));
 loDriver ld;		// structure of driver description
 LONG ecode;		// error code 
 tTotal = TAGS_NUM_MAX;		// total tag quantity
 if (my_service) {
      UL_ERROR((LOGID, "Driver already initialized!"));
      return 0;
  }
 memset(&ld, 0, sizeof(ld));   
 ld.ldRefreshRate =5000;		// polling time 
 ld.ldRefreshRate_min = 4000;	// minimum polling time
 ld.ldWriteTags = WriteTags;	// pointer to function write tag
 ld.ldReadTags = ReadTags;		// pointer to function read tag
 ld.ldSubscribe = activation_monitor;	// callback of tag activity
 ld.ldFlags = loDF_IGNCASE;				// ignore case
 ld.ldBranchSep = '.';					// hierarchial branch separator
 ecode = loServiceCreate(&my_service, &ld, tTotal);		//	creating loService 
 UL_TRACE((LOGID, "%!e loServiceCreate()=",ecode));	// write to log returning code
 if (ecode) return 1;									// error to create service	

 UL_INFO((LOGID, "init_tags(%d | %d)",devNum,tag_num));
 FILETIME ft;	
 UINT rights=0;	
 WCHAR buf[DATALEN_MAX];
 LCID lcid;
 GetSystemTimeAsFileTime(&ft);	// retrieves the current system date and time

 UINT tag_add=0;
 for (UINT i=0; i<devNum; i++)
	{
	 UL_ERROR((LOGID, "(%d/%d) device%d",i,devNum,i));
	 UINT max = tag_num/devNum;
	 for (UINT r=0; r<max; r++)
		{
		 tn[tag_add] = new char[DATALEN_MAX];	// reserve memory for massive
		 sprintf(tn[tag_add],"COM%d.UnknownDevice%d.param%d",1,i,r);
		 rights=OPC_READABLE | OPC_WRITEABLE;
		 VariantInit(&tv[tag_add].tvValue);
		 lcid = MAKELCID(0x0409, SORT_DEFAULT); 
 		 MultiByteToWideChar(CP_ACP, 0,tn[tag_add], strlen(tn[tag_add])+1,	buf, sizeof(buf)/sizeof(buf[0])); 

		 if (r==0)
			{
			 V_I2(&tv[tag_add].tvValue) = 0;
			 V_VT(&tv[tag_add].tvValue) = VT_I2;
			}
		 if (r>0)
			{
			 V_R4(&tv[tag_add].tvValue) = 0.0;
			 V_VT(&tv[tag_add].tvValue) = VT_R4;
			}
		 ecode = loAddRealTag_aW(my_service, &ti[tag_add], (loRealTag)(tag_add), buf, 0, rights, &tv[tag_add].tvValue, 0, 0); 
		 tv[tag_add].tvTi = ti[tag_add];
		 tv[tag_add].tvState.tsTime = ft;
		 tv[tag_add].tvState.tsError = S_OK;
		 tv[tag_add].tvState.tsQuality = OPC_QUALITY_NOT_CONNECTED;
		 UL_TRACE((LOGID, "%!e loAddRealTag(%s) = %u [%d]", ecode, tn[tag_add], ti[tag_add], tag_add));
		 printf ("loAddRealTag(%s) = %u [%d]\n", tn[tag_add], ti[tag_add], tag_add);
		 if (ti[tag_add]>=tag_add) tag_add++;
		}
	}
  tag_num=tag_add;
  UL_ERROR((LOGID, "%!e driver_init()=", ecode));
  LeaveCriticalSection(&lk_values);

  return 0;
}
//----------------------------------------------------------------------------------------------
UINT DestroyDriver()
{
  if (my_service)
    {
      INT ecode = loServiceDestroy(my_service);
      UL_INFO((LOGID, "%!e loServiceDestroy(%p) = ", ecode));	// destroy derver
      DeleteCriticalSection(&lk_values);						// destroy CS
      my_service = 0;		
    }
 return	1;
}
//----------------------------------------------------------------------------------------------
VOID poll_device()
{
 UL_DEBUG((LOGID, "poll_device ()"));
 FILETIME ft; INT ecode=0;

 GetSystemTimeAsFileTime(&ft);
 EnterCriticalSection(&lk_values);
 UL_DEBUG((LOGID, "Data to tag (%d)",tag_num));
 printf ("Data to tag (%d)\n",tag_num);
 for (UINT ci=0;ci<tag_num; ci++)
	{
	 UINT i2value=0;
	 FLOAT r4value=0.0;
	 i2value=rand() % 10;           // случайное целое число
     r4value=(FLOAT)rand()/100;		// случайное число с плавающей запятой

	 VARTYPE tvVt = tv[ci].tvValue.vt;
	 VariantClear(&tv[ci].tvValue);	  
	 switch (tvVt)
		{
		 case VT_I2: V_I2(&tv[ci].tvValue) = (UINT) i2value; printf ("[%d] %s (%d)\n",ci,tn[ci], i2value); break;
		 case VT_R4: V_R4(&tv[ci].tvValue) = (FLOAT) r4value; printf ("[%d] %s (%f)\n",ci,tn[ci], r4value); break;
		}

     V_VT(&tv[ci].tvValue) = tvVt;
	 tv[ci].tvState.tsQuality = OPC_QUALITY_GOOD;
	 tv[ci].tvState.tsTime = ft;
	}
 loCacheUpdate(my_service, tag_num, tv, 0);
 LeaveCriticalSection(&lk_values);
 Sleep(100);
 UL_DEBUG((LOGID, "loCacheUpdate complete (%d)",tag_num));
 Sleep(3000);
 UL_DEBUG((LOGID, "poll_device ()"));
}
