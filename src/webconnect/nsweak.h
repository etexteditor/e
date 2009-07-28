///////////////////////////////////////////////////////////////////////////////
// Name:        nsweak.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSWEAK_H
#define __WXWEBCONNECT_NSWEAK_H


class nsSupportsWeakReferenceBase : public nsISupportsWeakReference
{
public:

    virtual void OnWeakReferenceDestroyed() = 0;
};


class nsWeakReference : public nsIWeakReference
{
public:

    nsWeakReference(nsSupportsWeakReferenceBase* obj)
    {
        m_w_obj = obj;
        m_w_ref_count = 0;
    }
    
    ~nsWeakReference()
    {
        if (m_w_obj)
        {
            m_w_obj->OnWeakReferenceDestroyed();
        }
    }
    
    NS_IMETHOD QueryReferent(const nsIID& iid, void** result)
    {
        if (result)
            *result = 0;
            
        if (!m_w_obj)
            return NS_ERROR_NULL_POINTER;
            
        return m_w_obj->QueryInterface(iid, result);
    }
    
    void Clear()
    {
        m_w_obj = 0;
    }
    
    // nsISupports implementation
    NS_IMETHOD QueryInterface(const nsIID& iid, void** result)
    {
        *result = NULL;
        
        if (iid.Equals(NS_GET_IID(nsIWeakReference)))
        {
            *result = (void*)(nsIWeakReference*)this;
        }
         else
        {
            return NS_ERROR_NO_INTERFACE;
        }
        
        this->AddRef();
        return NS_OK;
     }

    NS_IMETHOD_(nsrefcnt) AddRef()
    {
        return ++m_w_ref_count;
    }

    NS_IMETHOD_(nsrefcnt) Release()
    {
        if (--m_w_ref_count == 0)
        {
            delete this;
            return 0;
        }
        return m_w_ref_count;
    }

private:

    nsSupportsWeakReferenceBase* m_w_obj;
    nsrefcnt m_w_ref_count;
};


class nsSupportsWeakReference : public nsSupportsWeakReferenceBase
{
public:

    nsSupportsWeakReference()
    {
        m_weak = NULL;
    }
    
    ~nsSupportsWeakReference()
    {
        if (m_weak)
            m_weak->Clear();
    }

    NS_IMETHOD GetWeakReference(nsIWeakReference** result)
    {
        if (!result)
            return NS_ERROR_NULL_POINTER;

        if (!m_weak)
        {
            m_weak = new nsWeakReference(this);
            if (!m_weak)
            {
                *result = NULL;
                return NS_ERROR_OUT_OF_MEMORY;
            }
        }
        
        m_weak->AddRef();
        
        *result = static_cast<nsIWeakReference*>(m_weak);
        
        return NS_OK;
    }

    void OnWeakReferenceDestroyed()
    {
        m_weak = NULL;
    }
    
private:

    nsWeakReference* m_weak;
};


inline nsIWeakReference* NS_GetWeakReference(nsISupports* obj, nsresult* error = NULL)
{
    nsIWeakReference* result = NULL;

    if (!obj)
    {
        if (error)
            *error = NS_ERROR_NULL_POINTER;
        return NULL;
    }
    
    nsISupportsWeakReference* ptr = NULL;
    obj->QueryInterface(nsISupportsWeakReference::GetIID(), (void**)&ptr);
    if (!ptr)
    {
        if (error)
            *error = NS_ERROR_NULL_POINTER;
        return NULL;
    }
    

    nsresult retval = ptr->GetWeakReference(&result);
    
    ptr->Release();
    
    if (error)
        *error = retval;
        
    return result;
}


#endif

