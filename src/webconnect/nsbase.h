///////////////////////////////////////////////////////////////////////////////
// Name:        nsbase.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-10
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSBASE_H
#define __WXWEBCONNECT_NSBASE_H

    
#ifdef WIN32
#define XP_WIN
#endif



///////////////////////////////////////////////////////////////////////////////
//  types, forwards, etc
///////////////////////////////////////////////////////////////////////////////


class nsAString;
class nsACString;
class nsISupports;
class nsIServiceManager;
class nsIComponentManager;
class nsIComponentRegistrar;
class nsIDirectoryServiceProvider;
class nsIFile;
class nsILocalFile;
struct nsStringContainer;
struct nsCStringContainer;

typedef size_t PRSize;
typedef unsigned char PRUint8;
typedef unsigned short int PRUint16;
typedef short int PRInt16;
typedef unsigned int PRUint32;
typedef int PRInt32;
typedef int PRBool;
typedef PRUint32 nsresult;
typedef unsigned long nsrefcnt;


#ifdef _MSC_VER
typedef wchar_t PRUnichar;
typedef __int64 PRInt64;
typedef unsigned __int64 PRUint64;
#else
typedef PRUint16 PRUnichar;
typedef long long PRInt64;
typedef unsigned long long PRUint64;
#endif

typedef PRUint64 PRTime;



#define PR_CALLBACK




///////////////////////////////////////////////////////////////////////////////
//  constants
///////////////////////////////////////////////////////////////////////////////


const PRUint32 PR_UINT32_MAX = 4294967295U;
const PRBool PR_TRUE = 1;
const PRBool PR_FALSE = 0;
#define nsnull 0




///////////////////////////////////////////////////////////////////////////////
//  function declarations
///////////////////////////////////////////////////////////////////////////////


nsresult XPCOMGlueStartup(const char* xpcom_dll_path);
nsresult NS_InitXPCOM2(nsIServiceManager** result, nsIFile* bin_directory, nsIDirectoryServiceProvider* app_file_location_provider);
void*    NS_Alloc(PRSize size);
void     NS_Free(void* ptr);
nsresult NS_GetServiceManager(nsIServiceManager** result);
nsresult NS_GetComponentManager(nsIComponentManager** result);
nsresult NS_GetComponentRegistrar(nsIComponentRegistrar** result);
nsresult NS_StringContainerInit(nsStringContainer& str);
nsresult NS_StringContainerInit2(nsStringContainer& str, const PRUnichar* str_data, PRUint32 len = PR_UINT32_MAX, PRUint32 flags = 0);
void     NS_StringContainerFinish(nsStringContainer& str);
nsresult NS_NewNativeLocalFile(const nsACString& path, PRBool follow_links, nsILocalFile** result);
nsresult NS_StringSetData(nsAString& str, const PRUnichar* str_data, PRUint32 len);
PRUint32 NS_StringGetData(const nsAString& str, const PRUnichar** str_data, PRBool* terminated = NULL);
nsresult NS_CStringContainerInit(nsCStringContainer& str);
nsresult NS_CStringContainerInit2(nsCStringContainer& str, const char* str_data, PRUint32 len = PR_UINT32_MAX, PRUint32 flags = 0);
void     NS_CStringContainerFinish(nsCStringContainer& str);
PRUint32 NS_CStringGetData(const nsACString& str, const char** str_data, PRBool* terminated = NULL);




///////////////////////////////////////////////////////////////////////////////
//  macros and interface implementations
///////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
    #define NS_IMETHOD virtual nsresult __stdcall
    #define NS_IMETHOD_(retval) virtual retval __stdcall
    #define NS_IMETHODIMP nsresult __stdcall
    #define NS_IMETHODIMP_(retval) retval __stdcall
    #define NS_CALLBACK(func) nsresult __stdcall func
#else
    #define NS_IMETHOD virtual nsresult
    #define NS_IMETHOD_(retval) virtual retval
    #define NS_IMETHODIMP nsresult
    #define NS_IMETHODIMP_(retval) retval
    #define NS_CALLBACK(func) nsresult func
#endif



#if (!defined(_MSC_VER) || (_MSC_VER > 600))
  #define NS_SPECIALIZE_TEMPLATE  template <>
#else
  #define NS_SPECIALIZE_TEMPLATE
#endif


#define NS_REINTERPRET_CAST(type,obj) reinterpret_cast<type>(obj)
#define NS_STATIC_CAST(type,obj) static_cast<type>(obj)

#define NS_SUCCEEDED(result) (((result) & 0x80000000) == 0)
#define NS_FAILED(result) ((result & 0x80000000))
#define NS_LIKELY(x)
#define NS_UNLIKELY(x)

#define NS_INIT_ISUPPORTS()
#define NS_FASTCALL
#define NS_CONSTRUCTOR_FASTCALL

#define NS_COM
#define NS_COM_GLUE

#define HAVE_CPP_ACCESS_CHANGING_USING

#define NS_ENSURE_ARG_POINTER(arg)
#define NS_PRECONDITION(x,y)

#define NS_ERROR_ABORT ((nsresult)0x80004004L)
#define NS_ERROR_FAILURE ((nsresult)0x80004005)
#define NS_ERROR_INVALID_POINTER ((nsresult)0x80004003)
#define NS_ERROR_NULL_POINTER NS_ERROR_INVALID_POINTER
#define NS_ERROR_OUT_OF_MEMORY ((nsresult)0x8007000e)
#define NS_ERROR_NO_INTERFACE ((nsresult)0x80004002)
#define NS_ERROR_NO_AGGREGATION ((nsresult)0x80040110)
#define NS_ERROR_NOT_IMPLEMENTED ((nsresult)0x80004001)
#define NS_ERROR_BASE ((nsresult)0xC1F30000)
#define NS_ERROR_NOT_INITIALIZED (NS_ERROR_BASE + 1)
#define NS_OK ((nsresult)0)


#define NS_GET_IID(iface) nsGetIID<iface>::GetIID()


#define NS_DECL_ISUPPORTS \
public: \
    NS_IMETHOD QueryInterface(const nsIID& iid, void** result); \
    NS_IMETHOD_(nsrefcnt) AddRef(); \
    NS_IMETHOD_(nsrefcnt) Release(); \
private: \
    refcount_holder m_refcount_holder; \
public:


#define NS_IMPL_ADDREF(class_name) \
    NS_IMETHODIMP_(nsrefcnt) class_name::AddRef() { \
        return ++m_refcount_holder.ref_count; \
    }

#define NS_IMPL_RELEASE(class_name) \
    NS_IMETHODIMP_(nsrefcnt) class_name::Release() { \
        if (--m_refcount_holder.ref_count == 0) { \
            delete this; \
            return 0; \
        } \
        return m_refcount_holder.ref_count; \
    }

#define NS_INTERFACE_MAP_BEGIN(class_name) \
    NS_IMETHODIMP class_name::QueryInterface(const nsIID& iid, void** result) { \
        *result = 0; \

#define NS_INTERFACE_MAP_ENTRY(iface) \
        if (iid.Equals(NS_GET_IID(iface))) { \
            this->AddRef(); \
            *result = static_cast<void*>(static_cast<iface*>(this)); \
            return NS_OK; \
        }
             
#define NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(iface1, iface2) \
        if (iid.Equals(NS_GET_IID(iface1))) { \
            this->AddRef(); \
            *result = static_cast<void*>(static_cast<iface1*>(static_cast<iface2*>(this))); \
            return NS_OK; \
        }
        
#define NS_INTERFACE_MAP_END \
        return NS_ERROR_NO_INTERFACE; \
    }


#define NS_IMPL_ISUPPORTS1(class_name, iface) \
    NS_IMPL_ADDREF(class_name) \
    NS_IMPL_RELEASE(class_name) \
    NS_IMETHODIMP class_name::QueryInterface(const nsIID& iid, void** result) { \
        *result = 0; \
        if (iid.Equals(NS_GET_IID(iface))) { \
            this->AddRef(); \
            *result = static_cast<void*>(static_cast<iface*>(this)); \
            return NS_OK; \
        } \
        if (iid.Equals(NS_GET_IID(nsISupports))) { \
            this->AddRef(); \
            *result = static_cast<void*>(static_cast<nsISupports*>(static_cast<iface*>(this))); \
            return NS_OK; \
        } \
        return NS_ERROR_NO_INTERFACE; \
    }


#ifdef WIN32
#define NS_DECLARE_STATIC_IID_ACCESSOR(iid) \
    static const nsIID& GetIID() { \
        static nsIID nsiid = iid; \
        return nsiid; \
    }
#else
#define NS_DECLARE_STATIC_IID_ACCESSOR(...) \
    static const nsIID& GetIID() { \
        static nsIID nsiid = __VA_ARGS__; \
        return nsiid; \
    }
#endif
    
#define NS_DEFINE_STATIC_IID_ACCESSOR(iid) NS_DECLARE_STATIC_IID_ACCESSOR(iid)
//#define NS_DEFINE_STATIC_IID_ACCESSOR(iface, iid)


class refcount_holder
{
public:

    long ref_count;

    refcount_holder()
    {
        ref_count = 0;
    }
};



template <class T>
inline nsrefcnt ns_if_addref(T ptr)
{
    if (ptr)
        return ptr->AddRef();
    return 0;
}
#define NS_IF_ADDREF(ptr) ns_if_addref((ptr))
#define NS_ADDREF(ptr) (ptr)->AddRef();




///////////////////////////////////////////////////////////////////////////////
//  iid stuff
///////////////////////////////////////////////////////////////////////////////


struct nsID
{
    PRUint32 m0;
    PRUint16 m1;
    PRUint16 m2;
    PRUint8 m3[8];

    PRBool Equals(const nsID& c) const
    {
        size_t i;
        if (m0 != c.m0)
            return PR_FALSE;
        if (m1 != c.m1)
            return PR_FALSE;
        if (m2 != c.m2)
            return PR_FALSE;
        for (i = 0; i < 8; ++i)
        {
            if (m3[i] != c.m3[i])
                return PR_FALSE;
        }
        return PR_TRUE;
    }
};


typedef nsID nsIID;
typedef nsID nsCID;
#define REFNSIID const nsIID&


// helper class because nsISupports doesn't have a
// built-in GetIID()

template <class T>
struct nsGetIID
{
    static const nsIID& GetIID()
    {
        return T::GetIID();
    }
};


NS_SPECIALIZE_TEMPLATE
struct nsGetIID<nsISupports>
{
    static const nsIID& GetIID()
    {
        static nsIID iid = /*NS_ISUPPORTS_IID*/
                     { 0x00000000, 0x0000, 0x0000,
                       {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
                     };
        return iid;
    }
};


// these are here so that if the gecko sdk is used,
// all the other headers are not dragged in
#define nsXPCOM_h__
#define nsTraceRefcnt_h___
#define nscore_h___
#define nsISupportsImpl_h__
#define nsError_h__
#define nsID_h__
#define nsDebug_h___
#define nsCOMPtr_h___
#define nsStringAPI_h__
#define nsISupportsUtils_h__
#endif

