///////////////////////////////////////////////////////////////////////////////
// Name:        nsstr.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSSTR_H
#define __WXWEBCONNECT_NSSTR_H


struct nsStringContainer
{
    void* v;
    void* d1;
    PRUint32 d2;
    void* d3;
};


struct nsCStringContainer
{
    void* v;
    void* d1;
    PRUint32 d2;
    void* d3;
};


class nsAString
{
protected:

    nsAString() { }
    ~nsAString() { }
};


class nsACString
{
protected:

    nsACString() { }
    ~nsACString() { }
};


class nsString : public nsAString,
                 public nsStringContainer
{
public:
    nsString()
    {
        NS_StringContainerInit(*this);
    }
    
    nsString(const PRUnichar* data, size_t len, PRUint32 flags)
    {
        NS_StringContainerInit2(*this, data, len, flags);
    }
    
    ~nsString()
    {
        NS_StringContainerFinish(*this);
    }
    
    void Assign(const PRUnichar* str, int len = PR_UINT32_MAX)
    {
        NS_StringSetData(*this, str, len);
    }
};


class nsCString : public nsACString,
                  public nsCStringContainer
{
public:
    nsCString()
    {
        NS_CStringContainerInit(*this);
    }
    
    ~nsCString()
    {
        NS_CStringContainerFinish(*this);
    }
    
    nsCString(const char* data, size_t len, PRUint32 flags)
    {
        NS_CStringContainerInit2(*this, data, len, flags);
    }
    
    const char* get() const
    {
        const char* ptr;
        NS_CStringGetData(*this, &ptr);
        return ptr;
    }
    
    void Assign(const char* str, int len = PR_UINT32_MAX)
    {
        NS_CStringContainerFinish(*this);
        NS_CStringContainerInit2(*this, str, len);
    }
};


class nsDependentString : public nsString
{
public:
    nsDependentString(const PRUnichar* data, size_t len = PR_UINT32_MAX)
           : nsString(data, len, 0x02/*NS_STRING_CONTAINER_INIT_DEPEND*/)
    {
    }
    
#ifndef _MSC_VER
    // for platforms with 4-byte wchar_t
    nsDependentString(const wchar_t* wdata, size_t specified_len = PR_UINT32_MAX)
           : nsString()
    {
        size_t i, len = 0;
        // determine length
        while (*(wdata+len))
            len++;
        if (specified_len != PR_UINT32_MAX && specified_len < len)
            len = specified_len;
        PRUnichar* data = new PRUnichar[len+1];
        for (i = 0; i < len; ++i)
            data[i] = wdata[i];
        Assign(data, len);
        delete[] data;
    }
#endif

};


class nsDependentCString : public nsCString
{
public:
    nsDependentCString(const char* data, size_t len = PR_UINT32_MAX)
           : nsCString(data, len, 0x02/*NS_CSTRING_CONTAINER_INIT_DEPEND*/)
    {
    }
};


typedef nsString nsEmbedString;
typedef nsCString nsEmbedCString;


#define NS_LITERAL_STRING(s) nsDependentString(L##s)
#define NS_LITERAL_CSTRING(s) nsDependentCString(s)


#endif

