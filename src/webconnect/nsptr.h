///////////////////////////////////////////////////////////////////////////////
// Name:        nsptr.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSPTR_H
#define __WXWEBCONNECT_NSPTR_H


class ns_smartptr_unknown
{
public:
    virtual nsresult qi(const nsIID& iid, void** result) const = 0;
};

template <class T>
class ns_smartptr : public ns_smartptr_unknown
{
public:

    T* p;

public:

    ns_smartptr()
    {
        p = 0;
    }

    ns_smartptr(T* const& _p)
    {
        p = _p;
        if (p)
        {
            p->AddRef();
        }
    }
    
    ns_smartptr(const ns_smartptr<T>& c)
    {
        p = c.p;
        if (p)
        {
            p->AddRef();
        }
    }
    
    ns_smartptr(const ns_smartptr_unknown& u)
    {
        u.qi(T::GetIID(), (void**)&p);
    }
    
    ~ns_smartptr()
    {
        if (p)
        {
            p->Release();
        }
    }

    operator nsISupports**() const
    {
        return static_cast<nsISupports*>(p);
    }
    
    operator T*() const
    {
        return p;
    }

    T* operator->() const
    {
        return p;
    }

    bool operator!() const
    {
        return (p == 0);
    }

    operator bool() const
    {
        return (p != 0);
    }

    bool operator()() const
    {
        return (p != 0);
    }


    ns_smartptr<T>& operator=(const ns_smartptr<T>& c)
    {
        if (p)
        {
            p->Release();
        }

        p = c.p;
        
        if (p)
        {
            p->AddRef();
        }
        
        return *this;
    }

    ns_smartptr<T>& operator=(T* _p)
    {
        if (p)
        {
            p->Release();
        }

        p = _p;

        if (p)
        {
            p->AddRef();
        }

        return *this;
    }

    ns_smartptr<T>& operator=(const ns_smartptr_unknown& u)
    {
        if (p)
        {
            p->Release();
            p = 0;
        }
        
        u.qi(T::GetIID(), (void**)&p);
        
        return *this;
    }

    bool empty() const
    {
        return (p == 0);
    }

    void clear()
    {
        if (p)
        {
            p->Release();
        }
        p = 0;
    }
    
    nsresult qi(const nsIID& iid, void** result) const
    {
        if (!p)
        {
            *result = 0;
            return NS_ERROR_NO_INTERFACE;
        }
        
        return p->QueryInterface(iid, result);
    }
};


class nsRequestInterface : public ns_smartptr_unknown
{
public:

    nsRequestInterface(nsISupports* p)
    {
        ptr.p = p;
        p->AddRef();
    }
    
    nsRequestInterface(ns_smartptr_unknown& u)
    {
        static nsIID nsISupportsIID = NS_ISUPPORTS_IID;
        u.qi(nsISupportsIID, (void**)&ptr.p);
    }
    
    nsresult qi(const nsIID& iid, void** result) const
    {
        if (ptr)
        {
            ns_smartptr<nsIInterfaceRequestor> factory_ptr = ptr;
            if (!factory_ptr)
            {
                *result = 0;
                return NS_ERROR_NO_INTERFACE;
            }

            return factory_ptr->GetInterface(iid, result);
        }

        return NS_ERROR_NULL_POINTER;
    }
    
public:
    
    ns_smartptr<nsISupports> ptr;
};


class nsToSmart : public ns_smartptr_unknown
{
public:

    nsToSmart(nsISupports* _ptr) : ptr(_ptr) { }
    
    nsresult qi(const nsIID& iid, void** result) const
    {
        if (!ptr)
            return NS_ERROR_NULL_POINTER;
        
        return ptr->QueryInterface(iid, result);
    }
    
public:

    nsISupports* ptr;
};


#endif

