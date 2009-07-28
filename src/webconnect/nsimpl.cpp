///////////////////////////////////////////////////////////////////////////////
// Name:        nsimpl.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#define _CRT_SECURE_NO_WARNINGS


#include <string>
#include <wx/wx.h>
#include "webcontrol.h"
#include "nsinclude.h"


#ifdef __WXMSW__
#include <windows.h>
#else
#include <dlfcn.h>
#endif



///////////////////////////////////////////////////////////////////////////////
//  function declarations
///////////////////////////////////////////////////////////////////////////////


typedef nsresult (*UnusedFunc)();
typedef nsresult (*InitFunc)(nsIServiceManager** result, nsIFile* bin_directory, nsIDirectoryServiceProvider* app_file_location_provider);
typedef void*    (*AllocFunc)(PRSize size);
typedef void     (*FreeFunc)(void* ptr);
typedef nsresult (*GetServiceManagerFunc)(nsIServiceManager** result);
typedef nsresult (*GetComponentManagerFunc)(nsIComponentManager** result);
typedef nsresult (*GetComponentRegistrarFunc)(nsIComponentRegistrar** result);
typedef nsresult (*StringContainerInitFunc)(nsStringContainer& str);
typedef nsresult (*StringContainerInit2Func)(nsStringContainer& str, const PRUnichar* str_data, PRUint32 len, PRUint32 flags);
typedef void     (*StringContainerFinishFunc)(nsStringContainer& str);
typedef nsresult (*NewNativeLocalFileFunc)(const nsACString& path, PRBool follow_links, nsILocalFile** result);
typedef nsresult (*StringSetDataFunc)(nsAString& str, const PRUnichar* str_data, PRUint32 len);
typedef PRUint32 (*StringGetDataFunc)(const nsAString& str, const PRUnichar** str_data, PRBool* terminated);
typedef nsresult (*CStringContainerInitFunc)(nsCStringContainer& str);
typedef nsresult (*CStringContainerInit2Func)(nsCStringContainer& str, const char* str_data, PRUint32 len, PRUint32 flags);
typedef void     (*CStringContainerFinishFunc)(nsCStringContainer& str);
typedef PRUint32 (*CStringGetDataFunc)(const nsACString& str, const char** str_data, PRBool* terminated);


struct XPCOMFunctionTable
{
    PRUint32 version;
    PRUint32 size;

    InitFunc Init;
    UnusedFunc /*ShutdownFunc*/ Shutdown;
    GetServiceManagerFunc GetServiceManager;
    GetComponentManagerFunc GetComponentManager;
    GetComponentRegistrarFunc GetComponentRegistrar;
    UnusedFunc /*GetMemoryManagerFunc*/ GetMemoryManager;
    UnusedFunc /*NewLocalFileFunc*/ NewLocalFile;
    NewNativeLocalFileFunc NewNativeLocalFile;
    UnusedFunc /*RegisterXPCOMExitRoutineFunc*/ RegisterExitRoutine;
    UnusedFunc /*UnregisterXPCOMExitRoutineFunc*/ UnregisterExitRoutine;
    UnusedFunc /*GetDebugFunc*/ GetDebug;
    UnusedFunc /*GetTraceRefcntFunc*/ GetTraceRefcnt;
    StringContainerInitFunc StringContainerInit;
    StringContainerFinishFunc StringContainerFinish;
    StringGetDataFunc StringGetData;
    StringSetDataFunc StringSetData;
    UnusedFunc /*StringSetDataRangeFunc*/ StringSetDataRange;
    UnusedFunc /*StringCopyFunc*/ StringCopy;
    CStringContainerInitFunc CStringContainerInit;
    CStringContainerFinishFunc CStringContainerFinish;
    CStringGetDataFunc CStringGetData;
    UnusedFunc /*CStringSetDataFunc*/ CStringSetData;
    UnusedFunc /*CStringSetDataRangeFunc*/ CStringSetDataRange;
    UnusedFunc /*CStringCopyFunc*/ CStringCopy;
    UnusedFunc /*CStringToUTF16*/ CStringToUTF16;
    UnusedFunc /*UTF16ToCString*/ Utf16ToCString;
    UnusedFunc /*StringCloneDataFunc*/ StringCloneData;
    UnusedFunc /*CStringCloneDataFunc*/ CStringCloneData;
    AllocFunc Alloc;
    UnusedFunc /*ReallocFunc*/ Realloc;
    FreeFunc Free;
    StringContainerInit2Func StringContainerInit2;
    CStringContainerInit2Func CStringContainerInit2;
    UnusedFunc /*StringGetMutableDataFunc*/ StringGetMutableData;
    UnusedFunc /*CStringGetMutableDataFunc*/ CStringGetMutableData;
    UnusedFunc /*Init3Func*/ Init3;
};


static XPCOMFunctionTable funcs;

static bool isLibraryOk()
{
    return (funcs.Init != 0) ? true : false;
}

typedef nsresult (PR_CALLBACK *GetFrozenFunctionsFunc)(XPCOMFunctionTable *func_table,
                                                       const char* path);




#ifdef __WXMSW__
#define XPCOM_PATH_SEPARATOR L'\\'
#else
#define XPCOM_PATH_SEPARATOR L'/'
#endif



static void GetDependentLibraryList(const char* xpcom_dll_path, wxArrayString& arr)
{
    // first load dependent libraries
    wxString fname = wxString::FromAscii(xpcom_dll_path);
    wxString xpcom_dir;
    
    // remove a trailing slash, if any
    if (fname.Length() > 0 && fname.Last() == XPCOM_PATH_SEPARATOR)
        fname.RemoveLast();
    xpcom_dir = fname.BeforeLast(XPCOM_PATH_SEPARATOR);
    
    // load 'dependentlibs.list'
    wxString deplib_list_fname = xpcom_dir;
    deplib_list_fname += XPCOM_PATH_SEPARATOR;
    deplib_list_fname += wxT("dependentlibs.list");
    
    FILE* ff = fopen(deplib_list_fname.mbc_str(), "r");
    if (ff)
    {
        wxString full_path;
        char buf[1024];
        while (fgets(buf, 1000, ff))
        {
            // ignore empty lines and comment lines
            if (!buf[0] || buf[0] == '#')
                continue;
            
            full_path = xpcom_dir;
            full_path += XPCOM_PATH_SEPARATOR;
            full_path += wxString::FromAscii(buf);
            
            // remove trailing \n
            if (full_path.Length() > 1 && full_path.Last() == L'\n')
                full_path.RemoveLast();
                
            arr.Add(full_path);
        }
        
        fclose(ff);
    }
}


#ifdef __WXMSW__
nsresult XPCOMGlueStartup(const char* xpcom_dll_path)
{
    // load library dependencies
    wxArrayString deplibs;
    GetDependentLibraryList(xpcom_dll_path, deplibs);
    size_t i, count = deplibs.GetCount();
    for (i = 0; i < count; ++i)
    {
        HMODULE h = LoadLibraryExA(deplibs.Item(i).mbc_str(),
                               0,
                               LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    
    // now load the functions from xpcom.dll
    
    HMODULE h = LoadLibraryExA(xpcom_dll_path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
    
    GetFrozenFunctionsFunc f;
    f = (GetFrozenFunctionsFunc)GetProcAddress(h, "NS_GetFrozenFunctions");
    
    if (!f)
    {
        FreeLibrary(h);
        return NS_ERROR_FAILURE;
    }
    
    nsresult res;
    funcs.version = 1;
    funcs.size = sizeof(XPCOMFunctionTable);
    res = f(&funcs, xpcom_dll_path);
    
    if (NS_FAILED(res))
    {
        FreeLibrary(h);
        return NS_ERROR_FAILURE;
    }

    return 0;
}

#else

nsresult XPCOMGlueStartup(const char* xpcom_dll_path)
{
    // load library dependencies
    wxArrayString deplibs;
    GetDependentLibraryList(xpcom_dll_path, deplibs);
    size_t i, count = deplibs.GetCount();
    for (i = 0; i < count; ++i)
    {
        void* handle = dlopen(deplibs.Item(i).mbc_str(), RTLD_GLOBAL | RTLD_LAZY);
    }
    
    // now load the functions from libxpcom.so
    
    void* h = dlopen(xpcom_dll_path, RTLD_GLOBAL | RTLD_LAZY);
    if (!h)
        return NS_ERROR_FAILURE;
        
    GetFrozenFunctionsFunc f =
    f = (GetFrozenFunctionsFunc)dlsym(h, "NS_GetFrozenFunctions");
    if (!f)
    {
        dlclose(h);
        return NS_ERROR_FAILURE;
    }
    
    nsresult res;
    funcs.version = 1;
    funcs.size = sizeof(XPCOMFunctionTable);
    res = f(&funcs, xpcom_dll_path);
    
    if (NS_FAILED(res))
    {
        dlclose(h);
        return NS_ERROR_FAILURE;
    }

    return 0;
}

#endif


nsresult NS_InitXPCOM2(nsIServiceManager** result,
                       nsIFile* bin_directory,
                       nsIDirectoryServiceProvider* app_file_location_provider)
{
    if (!funcs.Init)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.Init(result, bin_directory, app_file_location_provider);
}

void* NS_Alloc(PRSize size)
{
    if (!funcs.Alloc)
        return NULL;
        
    return funcs.Alloc(size);
}

void NS_Free(void* ptr)
{
    if (!funcs.Free)
        return;
    
    funcs.Free(ptr);
}

nsresult NS_GetServiceManager(nsIServiceManager** result)
{
    if (!funcs.GetServiceManager)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.GetServiceManager(result);
}

nsresult NS_GetComponentManager(nsIComponentManager** result)
{
    if (!funcs.GetComponentManager)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.GetComponentManager(result);
}

nsresult NS_GetComponentRegistrar(nsIComponentRegistrar** result)
{
    if (!funcs.GetComponentRegistrar)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.GetComponentRegistrar(result);
}

nsresult NS_StringContainerInit(nsStringContainer& str)
{
    if (!funcs.StringContainerInit)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringContainerInit(str);
}

nsresult NS_StringContainerInit2(nsStringContainer& str,
                                 const PRUnichar* str_data,
                                 PRUint32 len,
                                 PRUint32 flags)
{
    if (!funcs.StringContainerInit2)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringContainerInit2(str, str_data, len, flags);
}

void NS_StringContainerFinish(nsStringContainer& str)
{
    if (!funcs.StringContainerFinish)
        return;

    funcs.StringContainerFinish(str);
}

nsresult NS_NewNativeLocalFile(const nsACString& path,
                               PRBool follow_links,
                               nsILocalFile** result)
{
    if (!funcs.NewNativeLocalFile)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.NewNativeLocalFile(path, follow_links, result);
}

nsresult NS_StringSetData(nsAString& str,
                          const PRUnichar* str_data,
                          PRUint32 len)
{
    if (!funcs.StringSetData)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringSetData(str, str_data, len);
}

PRUint32 NS_StringGetData(const nsAString& str,
                          const PRUnichar** str_data,
                          PRBool* terminated)
{
    if (!funcs.StringGetData)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringGetData(str, str_data, terminated);
}

nsresult NS_CStringContainerInit(nsCStringContainer& str)
{
    if (!funcs.CStringContainerInit)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.CStringContainerInit(str);
}

nsresult NS_CStringContainerInit2(nsCStringContainer& str,
                                  const char* str_data,
                                  PRUint32 len,
                                  PRUint32 flags)
{
    if (!funcs.CStringContainerInit2)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.CStringContainerInit2(str, str_data, len, flags);
}

void NS_CStringContainerFinish(nsCStringContainer& str)
{
    if (!funcs.CStringContainerFinish)
        return;

    funcs.CStringContainerFinish(str);
}

PRUint32 NS_CStringGetData(const nsACString& str,
                           const char** str_data,
                           PRBool* terminated)
{
    if (!funcs.CStringGetData)
        return 0;

    return funcs.CStringGetData(str, str_data, terminated);
}




///////////////////////////////////////////////////////////////////////////////
//  utility functions
///////////////////////////////////////////////////////////////////////////////


wxString ns2wx(nsEmbedCString& str)
{
    wxString res;
    res = wxString::FromAscii(str.get());
    return res;
}


wxString ns2wx(nsEmbedString& str)
{
    wxString res;
    const PRUnichar* begin;
    const PRUnichar* end;
    
    PRUint32 i, len = NS_StringGetData(str, &begin);
    end = begin + len;
    
    res.Alloc(end - begin);
    for (i = 0; i < len; ++i)
        res += (wxChar)(*(begin+i));
    
    return res;
}

wxString ns2wx(const PRUnichar* str)
{
    if (!str)
        return wxT("");
        
    wxString res;
    size_t i, len = 0;
    while (*(str+len))
        len++;
    res.Alloc(len);
    for (i = 0; i < len; ++i)
        res += *(str+i);
    return res;
}

void wx2ns(const wxString& wxstr, nsEmbedString& nsstr)
{
    size_t i, len = wxstr.Length();
    PRUnichar* buf = new PRUnichar[len+1];
    for (i = 0; i < len; ++i)
        buf[i] = wxstr.GetChar(i);
    nsstr.Assign(buf, len);
    delete[] buf;
}

void wx2ns(const wxString& wxstr, nsEmbedCString& nsstr)
{
    nsstr.Assign(wxstr.mbc_str());
}

PRUnichar* wxToUnichar(const wxString& wxstr)
{
    size_t i,len = wxstr.Length();
    PRUnichar* ret = (PRUnichar*)NS_Alloc((len+1) * sizeof(PRUnichar));
    for (i = 0; i < len; ++i)
        *(ret+i) = (PRUnichar)wxstr.GetChar(i);
    *(ret+len) = 0;
    return ret;
}

void freeUnichar(PRUnichar* p)
{
    NS_Free((void*)p);
}

ns_smartptr<nsIWindowWatcher> nsGetWindowWatcherService()
{
    ns_smartptr<nsIServiceManager> service_mgr;
    ns_smartptr<nsIWindowWatcher> result;
    nsresult res;
    
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IWINDOWWATCHER_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/embedcomp/window-watcher;1",
                                        iid,
                                        (void**)&result.p);
    
    return result;
}

ns_smartptr<nsIPref> nsGetPrefService()
{
    ns_smartptr<nsIServiceManager> service_mgr;
    ns_smartptr<nsIPref> result;
    nsresult res;
    
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IPREF_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/preferences;1",
                                        iid,
                                        (void**)&result.p);
    
    return result;
}

ns_smartptr<nsIProperties> nsGetDirectoryService()
{
    ns_smartptr<nsIProperties> result;
    ns_smartptr<nsIServiceManager> service_mgr;
    nsresult res;
    
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IPROPERTIES_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/file/directory_service;1",
                                        iid,
                                        (void**)&result.p);
    
    return result;
}

ns_smartptr<nsIIOService> nsGetIOService()
{
    ns_smartptr<nsIIOService> result;
    ns_smartptr<nsIServiceManager> service_mgr;
    nsresult res;
    
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IIOSERVICE_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/network/io-service;1",
                                        iid,
                                        (void**)&result.p);
    
    return result;
}

ns_smartptr<nsISupports> nsGetService(const char* contract_id)
{
    ns_smartptr<nsISupports> result;
    ns_smartptr<nsIServiceManager> service_mgr;
    nsresult res;
    
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_ISUPPORTS_IID;
    service_mgr->GetServiceByContractID(contract_id,
                                        iid,
                                        (void**)&result.p);
    
    return result;
}

ns_smartptr<nsISupports> nsCreateInstance(const char* contract_id)
{
    ns_smartptr<nsISupports> res;
    static nsIID nsISupportsIID = NS_ISUPPORTS_IID;
    
    ns_smartptr<nsIComponentManager> comp_mgr;
    NS_GetComponentManager(&comp_mgr.p);
    if (comp_mgr)
    {
        comp_mgr->CreateInstanceByContractID(contract_id,
                                             0,
                                             nsISupportsIID,
                                             (void**)&res.p);
    }
    
    return res;
}

ns_smartptr<nsISupports> nsCreateInstance(const nsCID& cid)
{
    ns_smartptr<nsISupports> res;
    static nsIID nsISupportsIID = NS_ISUPPORTS_IID;
    
    ns_smartptr<nsIComponentManager> comp_mgr;
    NS_GetComponentManager(&comp_mgr.p);
    if (comp_mgr)
    {
        comp_mgr->CreateInstance(cid,
                                 0,
                                 nsISupportsIID,
                                 (void**)&res.p);
    }
    
    return res;
}

ns_smartptr<nsILocalFile> nsNewLocalFile(const wxString& filename)
{
    nsresult res = 0;
    ns_smartptr<nsILocalFile> ret;
    
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)filename.mbc_str()), TRUE, &ret.p);
    
    if (NS_FAILED(res))
        ret.clear();
    
    return ret;
}


ns_smartptr<nsIURI> nsNewURI(const wxString& spec)
{
    ns_smartptr<nsIURI> res;
    
    ns_smartptr<nsIIOService> io_service = nsGetIOService();
    if (io_service.empty())
        return res;
    
    std::string cstr_spec = (const char*)spec.mbc_str();
    
    io_service->NewURI(nsDependentCString(cstr_spec.c_str()), nsnull, nsnull, &res.p);
    
    return res;
}




///////////////////////////////////////////////////////////////////////////////
//  ProgressListenerAdaptor class implementation
///////////////////////////////////////////////////////////////////////////////


ProgressListenerAdaptor::ProgressListenerAdaptor(wxWebProgressBase* progress)
{
    m_progress = progress;
    
    if (m_progress)
        m_progress->OnStart();
}

ProgressListenerAdaptor::~ProgressListenerAdaptor()
{
}

NS_IMETHODIMP ProgressListenerAdaptor::Init(
                                           nsIURI* source,
                                           nsIURI* target,
                                           const nsAString& display_name,
                                           nsIMIMEInfo* mime_info,
                                           PRTime start_time,
                                           nsILocalFile* temp_file,
                                           nsICancelable* cancelable)
{
    return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor::OnStateChange(
                         nsIWebProgress* web_progress,
                         nsIRequest* request,
                         PRUint32 state_flags,
                         nsresult status)
{
    if (state_flags & nsIWebProgressListener::STATE_STOP)
    {
        if (m_progress)
            m_progress->OnFinish();
    }
    
    return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor::OnProgressChange(
                         nsIWebProgress* web_progress,
                         nsIRequest* request,
                         PRInt32 cur_self_progress,
                         PRInt32 max_self_progress,
                         PRInt32 cur_total_progress,
                         PRInt32 max_total_progress)
{
    return OnProgressChange64(web_progress,
                              request,
                              cur_self_progress,
                              max_self_progress,
                              cur_total_progress,
                              max_total_progress);
}

NS_IMETHODIMP ProgressListenerAdaptor::OnProgressChange64(
                         nsIWebProgress* web_progress,
                         nsIRequest* request,
                         PRInt64 cur_self_progress,
                         PRInt64 max_self_progress,
                         PRInt64 cur_total_progress,
                         PRInt64 max_total_progress)
{
    if (m_progress)
    {
        m_progress->OnProgressChange(wxLongLong(cur_self_progress),
                                     wxLongLong(max_self_progress));
                                     
        if (m_progress->IsCancelled())
            request->Cancel(0x804b0002 /*NS_BINDING_ABORTED*/);
    }
    
    return NS_OK;
}


NS_IMETHODIMP ProgressListenerAdaptor::OnLocationChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     nsIURI* location)
{
   return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor::OnStatusChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     nsresult status,
                     const PRUnichar* message)
{
    if (NS_FAILED(status))
    {
        if (m_progress && !m_progress->IsCancelled())
            m_progress->OnError(ns2wx(message));
    }
    
    return NS_OK;
}


NS_IMETHODIMP ProgressListenerAdaptor::OnSecurityChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     PRUint32 state)
{
   return NS_OK;
}


NS_IMPL_ADDREF(ProgressListenerAdaptor)
NS_IMPL_RELEASE(ProgressListenerAdaptor)

NS_INTERFACE_MAP_BEGIN(ProgressListenerAdaptor)
    NS_INTERFACE_MAP_ENTRY(nsISupports)
    NS_INTERFACE_MAP_ENTRY(nsITransfer)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener2)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
NS_INTERFACE_MAP_END

