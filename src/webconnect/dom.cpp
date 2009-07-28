///////////////////////////////////////////////////////////////////////////////
// Name:        dom.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-09-30
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>
#include "nsinclude.h"
#include "webcontrol.h"
#include "domprivate.h"



///////////////////////////////////////////////////////////////////////////////
//  wxDOMNode class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMNode::wxDOMNode
// Description: Creates a new wxDOMNode object.
//
// Syntax: wxDOMNode::wxDOMNode()
//
// Remarks: Creates a new wxDOMNode object.

wxDOMNode::wxDOMNode()
{
    m_data = new wxDOMNodeData;
}

wxDOMNode::~wxDOMNode()
{
    delete m_data;
}

wxDOMNode::wxDOMNode(const wxDOMNode& c)
{
    m_data = new wxDOMNodeData;
    assign(c);
}

wxDOMNode& wxDOMNode::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

void wxDOMNode::assign(const wxDOMNode& c)
{
    m_data->setNode(c.m_data->node_ptr.p);
}

// (METHOD) wxDOMNode::IsOk
// Description:
//
// Syntax: bool wxDOMNode::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Node is valid, and false otherwise.

bool wxDOMNode::IsOk() const
{
    return (m_data->node_ptr ? true : false);
}

// (METHOD) wxDOMNode::GetOwnerDocument
// Description:
//
// Syntax: wxDOMDocument wxDOMNode::GetOwnerDocument()
//
// Remarks:
//
// Returns:

wxDOMDocument wxDOMNode::GetOwnerDocument()
{
    wxDOMDocument node;

    if (!IsOk())
        return node;

    nsIDOMDocument* document = NULL;
    m_data->node_ptr->GetOwnerDocument(&document);
    
    if (!document)
        return node;
        
    node.m_data->setNode(document);
    document->Release();

    return node;
}

// (METHOD) wxDOMNode::GetNodeName
// Description:
//
// Syntax: wxString wxDOMNode::GetNodeName()
//
// Remarks:
//
// Returns:

wxString wxDOMNode::GetNodeName()
{
    wxString res;
    if (!IsOk())
        return res;
        
    nsEmbedString s;
    m_data->node_ptr->GetNodeName(s);
    res = ns2wx(s);
    
    return res;
}

// (METHOD) wxDOMNode::GetNodeType
// Description:
//
// Syntax: int wxDOMNode::GetNodeType()
//
// Remarks:
//
// Returns:

int wxDOMNode::GetNodeType()
{
    if (!IsOk())
        return 0;
        
    PRUint16 val = 0;
    m_data->node_ptr->GetNodeType(&val);
    return val;
}

// (METHOD) wxDOMNode::GetNodeValue
// Description:
//
// Syntax: wxString wxDOMNode::GetNodeValue()
//
// Remarks:
//
// Returns:

wxString wxDOMNode::GetNodeValue()
{
    wxString res;
    if (!IsOk())
        return res;
        
    nsEmbedString s;
    m_data->node_ptr->GetNodeValue(s);
    res = ns2wx(s);
    
    return res;
}

// (METHOD) wxDOMNode::SetNodeValue
// Description:
//
// Syntax: void wxDOMNode::SetNodeValue(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMNode::SetNodeValue(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->node_ptr->SetNodeValue(nsvalue);
}

// (METHOD) wxDOMNode::GetParentNode
// Description:
//
// Syntax: wxDOMNode wxDOMNode::GetParentNode()
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::GetParentNode()
{
    wxDOMNode node;
    if (!IsOk())
        return node;
        
    nsIDOMNode* result = NULL;
    m_data->node_ptr->GetParentNode(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::GetChildNodes
// Description:
//
// Syntax: wxDOMNodeList wxDOMNode::GetChildNodes()
//
// Remarks:
//
// Returns:

wxDOMNodeList wxDOMNode::GetChildNodes()
{
    wxDOMNodeList node_list;
    if (!IsOk())
        return node_list;

    m_data->node_ptr->GetChildNodes(&node_list.m_data->ptr.p);
    return node_list;
}

// (METHOD) wxDOMNode::GetFirstChild
// Description:
//
// Syntax: wxDOMNode wxDOMNode::GetFirstChild()
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::GetFirstChild()
{
    wxDOMNode node;
    if (!IsOk())
        return node;
        
    nsIDOMNode* result = NULL;
    m_data->node_ptr->GetFirstChild(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::GetLastChild
// Description:
//
// Syntax: wxDOMNode wxDOMNode::GetLastChild()
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::GetLastChild()
{
    wxDOMNode node;

    if (!IsOk())
        return node;

    nsIDOMNode* result = NULL;
    m_data->node_ptr->GetLastChild(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::GetPreviousSibling
// Description:
//
// Syntax: wxDOMNode wxDOMNode::GetPreviousSibling()
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::GetPreviousSibling()
{
    wxDOMNode node;

    if (!IsOk())
        return node;
        
    nsIDOMNode* result = NULL;
    m_data->node_ptr->GetPreviousSibling(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::GetNextSibling
// Description:
//
// Syntax: wxDOMNode wxDOMNode::GetNextSibling()
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::GetNextSibling()
{
    wxDOMNode node;

    if (!IsOk())
        return node;
        
    nsIDOMNode* result = NULL;
    m_data->node_ptr->GetNextSibling(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::InsertBefore
// Description:
//
// Syntax: wxDOMNode wxDOMNode::InsertBefore(wxDOMNode& new_child,
//                                           wxDOMNode& ref_child)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::InsertBefore(wxDOMNode& new_child,
                                  wxDOMNode& ref_child)
{
    wxDOMNode node;

    if (!IsOk())
        return node;

    nsIDOMNode* result = NULL;
    m_data->node_ptr->InsertBefore(new_child.m_data->node_ptr,
                                   ref_child.m_data->node_ptr,
                                   &result);

    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::ReplaceChild
// Description:
//
// Syntax: wxDOMNode wxDOMNode::ReplaceChild(wxDOMNode& new_child, 
//                                           wxDOMNode& old_child)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::ReplaceChild(wxDOMNode& new_child, 
                                  wxDOMNode& old_child)
{
    wxDOMNode node;

    if (!IsOk())
        return node;

    nsIDOMNode* result = NULL;
    m_data->node_ptr->ReplaceChild(new_child.m_data->node_ptr,
                                   old_child.m_data->node_ptr,
                                   &result);
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::RemoveChild
// Description:
//
// Syntax: wxDOMNode wxDOMNode::RemoveChild(wxDOMNode& old_child)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::RemoveChild(wxDOMNode& old_child)
{
    wxDOMNode node;

    if (!IsOk())
        return node;

    nsIDOMNode* result = NULL;
    m_data->node_ptr->RemoveChild(old_child.m_data->node_ptr,
                                  &result);
                                  
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::AppendChild
// Description:
//
// Syntax: wxDOMNode wxDOMNode::AppendChild(wxDOMNode& new_child)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::AppendChild(wxDOMNode& new_child)
{
    wxDOMNode node;

    if (!IsOk())
        return node;

    nsIDOMNode* result = NULL;
    m_data->node_ptr->AppendChild(new_child.m_data->node_ptr,
                                  &result);
                                  
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::GetAttributes
// Description:
//
// Syntax: wxDOMNamedNodeMap wxDOMNode::GetAttributes()
//
// Remarks:
//
// Returns:

wxDOMNamedNodeMap wxDOMNode::GetAttributes()
{
    wxDOMNamedNodeMap node_map;

    if (!IsOk())
        return node_map;

    m_data->node_ptr->GetAttributes(&node_map.m_data->ptr.p);
    return node_map;
}

// (METHOD) wxDOMNode::CloneNode
// Description:
//
// Syntax: wxDOMNode wxDOMNode::CloneNode(bool deep)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNode::CloneNode(bool deep)
{
    wxDOMNode node;

    if (!IsOk())
        return node;
        
    nsIDOMNode* result = NULL;
    m_data->node_ptr->CloneNode((deep ? PR_TRUE : PR_FALSE),
                                &result);

    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMNode::Normalize
// Description:
//
// Syntax: void wxDOMNode::Normalize()
//
// Remarks:
//
// Returns:

void wxDOMNode::Normalize()
{
    m_data->node_ptr->Normalize();
}

// (METHOD) wxDOMNode::IsSupported
// Description:
//
// Syntax: bool wxDOMNode::IsSupported(const wxString& feature, 
//                                     const wxString& version)
//
// Remarks:
//
// Returns:

bool wxDOMNode::IsSupported(const wxString& feature, 
                            const wxString& version)
{
    if (!IsOk())
        return false;

    nsEmbedString nsfeature, nsversion;
    wx2ns(feature, nsfeature);
    wx2ns(version, nsversion);

    PRBool result = PR_FALSE;
    m_data->node_ptr->IsSupported(nsfeature, nsversion, &result);

    if (result == PR_TRUE)
        return true;

    return false;
}

// (METHOD) wxDOMNode::HasChildNodes
// Description:
//
// Syntax: bool wxDOMNode::HasChildNodes()
//
// Remarks:
//
// Returns:

bool wxDOMNode::HasChildNodes()
{
    if (!IsOk())
        return false;
        
    PRBool result = PR_FALSE;
    m_data->node_ptr->HasChildNodes(&result);

    if (result == PR_TRUE)
        return true;

    return false;
}

// (METHOD) wxDOMNode::HasAttributes
// Description:
//
// Syntax: bool wxDOMNode::HasAttributes()
//
// Remarks:
//
// Returns:

bool wxDOMNode::HasAttributes()
{
    if (!IsOk())
        return false;
        
    PRBool result = PR_FALSE;
    m_data->node_ptr->HasAttributes(&result);

    if (result == PR_TRUE)
        return true;

    return false;
}

// (METHOD) wxDOMNode::GetPrefix
// Description:
//
// Syntax: wxString wxDOMNode::GetPrefix()
//
// Remarks:
//
// Returns:

wxString wxDOMNode::GetPrefix()
{
    wxString res;

    if (!IsOk())
        return res;
        
    nsEmbedString s;
    m_data->node_ptr->GetPrefix(s);
    res = ns2wx(s);

    return res;
}

// (METHOD) wxDOMNode::SetPrefix
// Description:
//
// Syntax: void wxDOMNode::SetPrefix(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMNode::SetPrefix(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->node_ptr->SetNodeValue(nsvalue);
}

// (METHOD) wxDOMNode::GetNamespaceURI
// Description:
//
// Syntax: wxString wxDOMNode::GetNamespaceURI()
//
// Remarks:
//
// Returns:

wxString wxDOMNode::GetNamespaceURI()
{
    wxString res;

    if (!IsOk())
        return res;
        
    nsEmbedString s;
    m_data->node_ptr->GetNamespaceURI(s);
    res = ns2wx(s);

    return res;
}

// (METHOD) wxDOMNode::GetLocalName
// Description:
//
// Syntax: wxString wxDOMNode::GetLocalName()
//
// Remarks:
//
// Returns:

wxString wxDOMNode::GetLocalName()
{
    wxString res;

    if (!IsOk())
        return res;
        
    nsEmbedString s;
    m_data->node_ptr->GetLocalName(s);
    res = ns2wx(s);

    return res;
}




// the following class provides an event listener for DOM events
// which converts those events to wx events


class wxDOMEventAdaptor : public nsIDOMEventListener
{
public:

    NS_DECL_ISUPPORTS
    
    NS_IMETHODIMP HandleEvent(nsIDOMEvent* dom_evt)
    {
        wxWebEvent evt(wxEVT_WEB_DOMEVENT, m_event_id);
        
        m_event_handler->ProcessEvent(evt);
        
        return NS_OK;
    }

public:
    wxEvtHandler* m_event_handler;
    int m_event_id;
};


NS_IMPL_ISUPPORTS1(wxDOMEventAdaptor, nsIDOMEventListener)




// (METHOD) wxDOMNode::AddEventListener
// Description:
//
// Syntax: bool wxDOMNode::AddEventListener(const wxString& type,
//                                          wxEvtHandler* event_handler,
//                                          int event_id,
//                                          bool use_capture)
//
// Remarks:
//
// Returns:

bool wxDOMNode::AddEventListener(const wxString& type,
                                 wxEvtHandler* event_handler,
                                 int event_id,
                                 bool use_capture)
{
    if (!IsOk())
        return false;
        
    ns_smartptr<nsIDOMEventTarget> evt_target = m_data->node_ptr;
    if (!evt_target)
        return false;
    
    wxASSERT(event_handler);
    
    wxDOMEventAdaptor* listener_adaptor = new wxDOMEventAdaptor;
    listener_adaptor->m_event_handler = event_handler;
    listener_adaptor->m_event_id = event_id;
    
    nsEmbedString nstype;
    wx2ns(type, nstype);
    
    if (NS_SUCCEEDED(evt_target->AddEventListener(nstype,
                                             listener_adaptor,
                                             use_capture ? PR_TRUE : PR_FALSE)))
    {
        return true;
    }

    return false;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMAttr class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMAttr::wxDOMAttr
// Description: Creates a new wxDOMAttr object.
//
// Syntax: wxDOMAttr::wxDOMAttr()
//
// Remarks: Creates a new wxDOMAttr object.

wxDOMAttr::wxDOMAttr() : wxDOMNode()
{
}

wxDOMAttr::wxDOMAttr(const wxDOMNode& node) : wxDOMNode(node)
{
}

wxDOMAttr& wxDOMAttr::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMAttr::IsOk
// Description:
//
// Syntax: bool wxDOMAttr::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Attribute is valid, and false otherwise.

bool wxDOMAttr::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    return (m_data->attr_ptr ? true : false);
}

// (METHOD) wxDOMAttr::GetName
// Description:
//
// Syntax: wxString wxDOMAttr::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMAttr::GetName()
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString ns;
    m_data->attr_ptr->GetName(ns);
    
    return ns2wx(ns);
}

// (METHOD) wxDOMAttr::GetSpecified
// Description:
//
// Syntax: bool wxDOMAttr::GetSpecified()
//
// Remarks:
//
// Returns:

bool wxDOMAttr::GetSpecified()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->attr_ptr->GetSpecified(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMAttr::GetValue
// Description:
//
// Syntax: wxString wxDOMAttr::GetValue()
//
// Remarks:
//
// Returns:

wxString wxDOMAttr::GetValue()
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString ns;
    m_data->attr_ptr->GetValue(ns);
    
    return ns2wx(ns);
}

// (METHOD) wxDOMAttr::GetOwnerElement
// Description:
//
// Syntax: wxDOMElement wxDOMAttr::GetOwnerElement()
//
// Remarks:
//
// Returns:

wxDOMElement wxDOMAttr::GetOwnerElement()
{
    wxDOMElement node;

    if (!IsOk())
        return node;

    nsIDOMElement* result = NULL;
    m_data->attr_ptr->GetOwnerElement(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMElement::wxDOMElement
// Description: Creates a new wxDOMElement object.
//
// Syntax: wxDOMElement::wxDOMElement()
//
// Remarks: Creates a new wxDOMElement object.

wxDOMElement::wxDOMElement() : wxDOMNode()
{
}

wxDOMElement::wxDOMElement(const wxDOMNode& node) : wxDOMNode(node)
{
}

wxDOMElement& wxDOMElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMElement::IsOk
// Description:
//
// Syntax: bool wxDOMElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Element is valid, and false otherwise.

bool wxDOMElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    return (m_data->element_ptr ? true : false);
}

// (METHOD) wxDOMElement::GetTagName
// Description:
//
// Syntax: wxString wxDOMElement::GetTagName()
//
// Remarks:
//
// Returns:

wxString wxDOMElement::GetTagName()
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString str;
    m_data->element_ptr->GetTagName(str);
    
    return ns2wx(str);
}

// (METHOD) wxDOMElement::GetAttribute
// Description:
//
// Syntax: wxString wxDOMElement::GetAttribute(const wxString& name)
//
// Remarks:
//
// Returns:

wxString wxDOMElement::GetAttribute(const wxString& name)
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString nsname, nsattribute;
    wx2ns(name, nsname);

    m_data->element_ptr->GetAttribute(nsname, nsattribute);

    return ns2wx(nsattribute);
}

// (METHOD) wxDOMElement::SetAttribute
// Description:
//
// Syntax: void wxDOMElement::SetAttribute(const wxString& name, 
//                                         const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMElement::SetAttribute(const wxString& name, 
                                const wxString& value)
{
    if (!IsOk())
        return;
    
    nsEmbedString nsname, nsvalue;
    wx2ns(name, nsname);
    wx2ns(value, nsvalue);

    m_data->element_ptr->SetAttribute(nsname, nsvalue);
}

// (METHOD) wxDOMElement::RemoveAttribute
// Description:
//
// Syntax: void wxDOMElement::RemoveAttribute(const wxString& name)
//
// Remarks:
//
// Returns:

void wxDOMElement::RemoveAttribute(const wxString& name)
{
    if (!IsOk())
        return;
    
    nsEmbedString nsname;
    wx2ns(name, nsname);

    m_data->element_ptr->RemoveAttribute(nsname);
}

// (METHOD) wxDOMElement::GetAttributeNode
// Description:
//
// Syntax: wxDOMAttr wxDOMElement::GetAttributeNode(const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMElement::GetAttributeNode(const wxString& name)
{
    wxDOMAttr node;

    if (!IsOk())
        return node;

    nsEmbedString nsname;
    wx2ns(name, nsname);

    nsIDOMAttr* result = NULL;
    m_data->element_ptr->GetAttributeNode(nsname, &result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMElement::SetAttributeNode
// Description:
//
// Syntax: wxDOMAttr wxDOMElement::SetAttributeNode(wxDOMAttr& new_attr)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMElement::SetAttributeNode(wxDOMAttr& new_attr)
{
    wxDOMAttr node;

    if (!IsOk())
        return node;

    nsIDOMAttr* result = NULL;
    m_data->element_ptr->SetAttributeNode(new_attr.m_data->attr_ptr,
                                          &result);

    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMElement::RemoveAttributeNode
// Description:
//
// Syntax: wxDOMAttr wxDOMElement::RemoveAttributeNode(wxDOMAttr& old_attr)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMElement::RemoveAttributeNode(wxDOMAttr& old_attr)
{
    wxDOMAttr node;

    if (!IsOk())
        return node;

    nsIDOMAttr* result = NULL;
    m_data->element_ptr->RemoveAttributeNode(old_attr.m_data->attr_ptr,
                                             &result);

    if (!result)
        return node;

    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMElement::GetElementsByTagName
// Description:
//
// Syntax: wxDOMNodeList wxDOMElement::GetElementsByTagName(const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMNodeList wxDOMElement::GetElementsByTagName(const wxString& name)
{
    wxDOMNodeList node_list;

    if (!IsOk())
        return node_list;

    nsEmbedString nsname;
    wx2ns(name, nsname);

    m_data->element_ptr->GetElementsByTagName(nsname,
                                              &node_list.m_data->ptr.p);
    return node_list;
}

// (METHOD) wxDOMElement::GetAttributeNS
// Description:
//
// Syntax: wxString wxDOMElement::GetAttributeNS(const wxString& namespace_uri,
//                                               const wxString& local_name)
//
// Remarks:
//
// Returns:

wxString wxDOMElement::GetAttributeNS(const wxString& namespace_uri,
                                      const wxString& local_name)
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString nsnamespace_uri, nslocal_name, nsattribute;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);

    m_data->element_ptr->GetAttributeNS(nsnamespace_uri,
                                        nslocal_name,
                                        nsattribute);

    return ns2wx(nsattribute);
}

// (METHOD) wxDOMElement::SetAttributeNS
// Description:
//
// Syntax: void wxDOMElement::SetAttributeNS(const wxString& namespace_uri,
//                                           const wxString& qualified_name,
//                                           const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMElement::SetAttributeNS(const wxString& namespace_uri,
                                  const wxString& qualified_name,
                                  const wxString& value)
{
    if (!IsOk())
        return;
    
    nsEmbedString nsnamespace_uri, nsqualified_name, nsvalue;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(qualified_name, nsqualified_name);

    m_data->element_ptr->SetAttributeNS(nsnamespace_uri,
                                        nsqualified_name,
                                        nsvalue);
}

// (METHOD) wxDOMElement::RemoveAttributeNS
// Description:
//
// Syntax: void wxDOMElement::RemoveAttributeNS(const wxString& namespace_uri,
//                                              const wxString& local_name)
//
// Remarks:
//
// Returns:

void wxDOMElement::RemoveAttributeNS(const wxString& namespace_uri,
                                     const wxString& local_name)
{
    if (!IsOk())
        return;
    
    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);

    m_data->element_ptr->RemoveAttributeNS(nsnamespace_uri, 
                                           nslocal_name);
}

// (METHOD) wxDOMElement::GetAttributeNodeNS
// Description:
//
// Syntax: wxDOMAttr wxDOMElement::GetAttributeNodeNS(const wxString& namespace_uri,
//                                                    const wxString& local_name)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMElement::GetAttributeNodeNS(const wxString& namespace_uri,
                                           const wxString& local_name)
{
    wxDOMAttr node;
 
    if (!IsOk())
        return node;

    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);

    nsIDOMAttr* result = NULL;
    m_data->element_ptr->GetAttributeNodeNS(nsnamespace_uri, 
                                            nslocal_name,
                                            &result);
                                            
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMElement::SetAttributeNodeNS
// Description:
//
// Syntax: wxDOMAttr wxDOMElement::SetAttributeNodeNS(wxDOMAttr& new_attr)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMElement::SetAttributeNodeNS(wxDOMAttr& new_attr)
{
    wxDOMAttr node;

    if (!IsOk())
        return node;

    nsIDOMAttr* result = NULL;
    m_data->element_ptr->SetAttributeNodeNS(new_attr.m_data->attr_ptr,
                                            &result);
                                            
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMElement::GetElementsByTagNameNS
// Description:
//
// Syntax: wxDOMNodeList wxDOMElement::GetElementsByTagNameNS(const wxString& namespace_uri,
//                                                            const wxString& local_name)
//
// Remarks:
//
// Returns:

wxDOMNodeList wxDOMElement::GetElementsByTagNameNS(const wxString& namespace_uri,
                                                   const wxString& local_name)
{
    wxDOMNodeList node_list;

    if (!IsOk())
        return node_list;

    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);

    m_data->doc_ptr->GetElementsByTagNameNS(nsnamespace_uri,
                                            nslocal_name,
                                            &node_list.m_data->ptr.p);
    return node_list;
}

// (METHOD) wxDOMElement::HasAttribute
// Description:
//
// Syntax: bool wxDOMElement::HasAttribute(const wxString& name)
//
// Remarks:
//
// Returns:

bool wxDOMElement::HasAttribute(const wxString& name)
{
    if (!IsOk())
        return false;

    nsEmbedString nsname;
    wx2ns(name, nsname);

    PRBool result = PR_FALSE;
    m_data->element_ptr->HasAttribute(nsname, &result);

    if (result == PR_TRUE)
        return true;

    return false;
}

// (METHOD) wxDOMElement::HasAttributeNS
// Description:
//
// Syntax: bool wxDOMElement::HasAttributeNS(const wxString& namespace_uri,
//                                           const wxString& local_name)
//
// Remarks:
//
// Returns:

bool wxDOMElement::HasAttributeNS(const wxString& namespace_uri,
                                  const wxString& local_name)
{
    if (!IsOk())
        return false;

    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);

    PRBool result = PR_FALSE;
    m_data->element_ptr->HasAttributeNS(nsnamespace_uri,
                                        nslocal_name,
                                        &result);

    if (result == PR_TRUE)
        return true;

    return false;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMText class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMText::wxDOMText
// Description: Creates a new wxDOMText object.
//
// Syntax: wxDOMText::wxDOMText()
//
// Remarks: Creates a new wxDOMText object.

wxDOMText::wxDOMText() : wxDOMNode()
{
}

wxDOMText::wxDOMText(const wxDOMNode& node) : wxDOMNode(node)
{
}

wxDOMText& wxDOMText::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMText::IsOk
// Description:
//
// Syntax: bool wxDOMText::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Text Node is valid, and false otherwise.

bool wxDOMText::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    return (m_data->text_ptr ? true : false);
}

// (METHOD) wxDOMText::SetData
// Description:
//
// Syntax: void wxDOMText::SetData(const wxString& data)
//
// Remarks:
//
// Returns:

void wxDOMText::SetData(const wxString& data)
{
    if (!IsOk())
        return;
    
    nsEmbedString ns;
    wx2ns(data, ns);
    
    m_data->text_ptr->SetData(ns);
}

// (METHOD) wxDOMText::GetData
// Description:
//
// Syntax: wxString wxDOMText::GetData()
//
// Remarks:
//
// Returns:

wxString wxDOMText::GetData()
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString ns;
    m_data->text_ptr->GetData(ns);
    
    return ns2wx(ns);
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMDocument class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMDocument::wxDOMDocument
// Description: Creates a new wxDOMDocument object.
//
// Syntax: wxDOMDocument::wxDOMDocument()
//
// Remarks: Creates a new wxDOMDocument object.

wxDOMDocument::wxDOMDocument() : wxDOMNode()
{
}

wxDOMDocument::wxDOMDocument(const wxDOMNode& node) : wxDOMNode(node)
{
}

wxDOMDocument& wxDOMDocument::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMDocument::IsOk
// Description:
//
// Syntax: bool wxDOMDocument::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Document Node is valid, and false otherwise.

bool wxDOMDocument::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    return (m_data->doc_ptr ? true : false);
}

// (METHOD) wxDOMDocument::GetDocumentElement
// Description:
//
// Syntax: wxDOMElement wxDOMDocument::GetDocumentElement()
//
// Remarks:
//
// Returns:

wxDOMElement wxDOMDocument::GetDocumentElement()
{
    wxDOMElement node;

    if (!IsOk())
        return node;

    nsIDOMElement* result = NULL;
    m_data->doc_ptr->GetDocumentElement(&result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMDocument::CreateElement
// Description:
//
// Syntax: wxDOMElement wxDOMDocument::CreateElement(const wxString& tag_name)
//
// Remarks:
//
// Returns:

wxDOMElement wxDOMDocument::CreateElement(const wxString& tag_name)
{
    wxDOMElement node;

    if (!IsOk())
        return node;
    
    nsEmbedString nstagname;
    wx2ns(tag_name, nstagname);
    
    nsIDOMElement* result = NULL;
    m_data->doc_ptr->CreateElement(nstagname, &result);

    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();
    
    return node;
}

// (METHOD) wxDOMDocument::CreateTextNode
// Description:
//
// Syntax: wxDOMText wxDOMDocument::CreateTextNode(const wxString& data)
//
// Remarks:
//
// Returns:

wxDOMText wxDOMDocument::CreateTextNode(const wxString& data)
{
    wxDOMText node;

    if (!IsOk())
        return node;
    
    nsEmbedString ns;
    wx2ns(data, ns);
    
    nsIDOMText* result = NULL;
    m_data->doc_ptr->CreateTextNode(ns, &result);
    
    if (!result)
        return node;
    
    node.m_data->setNode(result);
    result->Release();
    
    return node;
}

// (METHOD) wxDOMDocument::CreateAttribute
// Description:
//
// Syntax: wxDOMAttr wxDOMDocument::CreateAttribute(const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMDocument::CreateAttribute(const wxString& name)
{
    wxDOMAttr node;

    if (!IsOk())
        return node;
    
    nsEmbedString ns;
    wx2ns(name, ns);
    
    nsIDOMAttr* result = NULL;
    m_data->doc_ptr->CreateAttribute(ns, &result);

    if (!result)
        return node;
    
    node.m_data->setNode(result);
    result->Release();
    
    return node;
}

// (METHOD) wxDOMDocument::GetElementsByTagName
// Description:
//
// Syntax: wxDOMNodeList wxDOMDocument::GetElementsByTagName(const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMNodeList wxDOMDocument::GetElementsByTagName(const wxString& name)
{
    wxDOMNodeList node_list;

    if (!IsOk())
        return node_list;

    nsEmbedString nsname;
    wx2ns(name, nsname);

    m_data->doc_ptr->GetElementsByTagName(nsname, &node_list.m_data->ptr.p);
    return node_list;
}

// (METHOD) wxDOMDocument::ImportNode
// Description:
//
// Syntax: wxDOMNode wxDOMDocument::ImportNode(wxDOMNode& arg, 
//                                             bool deep)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMDocument::ImportNode(wxDOMNode& arg, 
                                    bool deep)
{
    wxDOMNode node;

    if (!IsOk())
        return node;

    nsIDOMNode* result;
    m_data->doc_ptr->ImportNode(arg.m_data->node_ptr,
                                (deep ? PR_TRUE : PR_FALSE),
                                &result);

    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();

    return node;
}

// (METHOD) wxDOMDocument::CreateElementNS
// Description:
//
// Syntax: wxDOMElement wxDOMDocument::CreateElementNS(const wxString& namespace_uri,
//                                                     const wxString& local_name)
//
// Remarks:
//
// Returns:

wxDOMElement wxDOMDocument::CreateElementNS(const wxString& namespace_uri,
                                            const wxString& local_name)
{
    wxDOMElement node;

    if (!IsOk())
        return node;
    
    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);
    
    nsIDOMElement* result = NULL;
    m_data->doc_ptr->CreateElementNS(nsnamespace_uri,
                                     nslocal_name,
                                     &result);

    if (!result)
        return node;
    
    node.m_data->setNode(result);
    result->Release();
    
    return node;
}

// (METHOD) wxDOMDocument::CreateAttributeNS
// Description:
//
// Syntax: wxDOMAttr wxDOMDocument::CreateAttributeNS(const wxString& namespace_uri,
//                                                    const wxString& local_name)
//
// Remarks:
//
// Returns:

wxDOMAttr wxDOMDocument::CreateAttributeNS(const wxString& namespace_uri,
                                           const wxString& local_name)
{
    wxDOMAttr node;

    if (!IsOk())
        return node;
    
    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);
    
    nsIDOMAttr* result = NULL;
    m_data->doc_ptr->CreateAttributeNS(nsnamespace_uri,
                                       nslocal_name,
                                       &result);

    if (!result)
        return node;
    
    node.m_data->setNode(result);
    result->Release();
    
    return node;
}

// (METHOD) wxDOMDocument::GetElementsByTagNameNS
// Description:
//
// Syntax: wxDOMNodeList wxDOMDocument::GetElementsByTagNameNS(const wxString& namespace_uri,
//                                                             const wxString& local_name)
//
// Remarks:
//
// Returns:

wxDOMNodeList wxDOMDocument::GetElementsByTagNameNS(const wxString& namespace_uri,
                                                    const wxString& local_name)
{
    wxDOMNodeList node_list;

    if (!IsOk())
        return node_list;

    nsEmbedString nsnamespace_uri, nslocal_name;
    wx2ns(namespace_uri, nsnamespace_uri);
    wx2ns(local_name, nslocal_name);

    m_data->doc_ptr->GetElementsByTagNameNS(nsnamespace_uri,
                                            nslocal_name,
                                            &node_list.m_data->ptr.p);
    return node_list;
}

// (METHOD) wxDOMDocument::GetElementById
// Description:
//
// Syntax: wxDOMElement wxDOMDocument::GetElementById(const wxString& id)
//
// Remarks:
//
// Returns:

wxDOMElement wxDOMDocument::GetElementById(const wxString& id)
{
    wxDOMElement node;

    if (!IsOk())
        return node;
    
    nsEmbedString nsid;
    wx2ns(id, nsid);

    nsIDOMElement* result = NULL;
    m_data->doc_ptr->GetElementById(nsid, &result);
    
    if (!result)
        return node;
        
    node.m_data->setNode(result);
    result->Release();
    
    return node;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLElement::wxDOMHTMLElement
// Description: Creates a new wxDOMHTMLElement object.
//
// Syntax: wxDOMHTMLElement::wxDOMHTMLElement()
//
// Remarks: Creates a new wxDOMHTMLElement object.

wxDOMHTMLElement::wxDOMHTMLElement() : wxDOMElement()
{
}

wxDOMHTMLElement::wxDOMHTMLElement(const wxDOMNode& node)  : wxDOMElement(node)
{
}

wxDOMHTMLElement& wxDOMHTMLElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Element is valid, and false otherwise.

bool wxDOMHTMLElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
    if (!m_data->element_ptr)
        return false;
        
    return (m_data->htmlelement_ptr ? true : false);
}

// (METHOD) wxDOMHTMLElement::GetId
// Description:
//
// Syntax: wxString wxDOMHTMLElement::GetId()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLElement::GetId()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->GetId(ns);
        return ns2wx(ns);
    }
    
    return wxEmptyString;
}

// (METHOD) wxDOMHTMLElement::SetId
// Description:
//
// Syntax: void wxDOMHTMLElement::SetId(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLElement::SetId(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->SetId(nsvalue);
    }
}

// (METHOD) wxDOMHTMLElement::GetTitle
// Description:
//
// Syntax: wxString wxDOMHTMLElement::GetTitle()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLElement::GetTitle()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->GetTitle(ns);
        return ns2wx(ns);
    }
    
    return wxEmptyString;
}

// (METHOD) wxDOMHTMLElement::SetTitle
// Description:
//
// Syntax: void wxDOMHTMLElement::SetTitle(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLElement::SetTitle(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->SetTitle(nsvalue);
    }
}

// (METHOD) wxDOMHTMLElement::GetLang
// Description:
//
// Syntax: wxString wxDOMHTMLElement::GetLang()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLElement::GetLang()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->GetLang(ns);
        return ns2wx(ns);
    }
    
    return wxEmptyString;
}

// (METHOD) wxDOMHTMLElement::SetLang
// Description:
//
// Syntax: void wxDOMHTMLElement::SetLang(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLElement::SetLang(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->SetLang(nsvalue);
    }
}

// (METHOD) wxDOMHTMLElement::GetDir
// Description:
//
// Syntax: wxString wxDOMHTMLElement::GetDir()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLElement::GetDir()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->GetDir(ns);
        return ns2wx(ns);
    }
    
    return wxEmptyString;
}

// (METHOD) wxDOMHTMLElement::SetDir
// Description:
//
// Syntax: void wxDOMHTMLElement::SetDir(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLElement::SetDir(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->SetDir(nsvalue);
    }
}

// (METHOD) wxDOMHTMLElement::GetClassName
// Description:
//
// Syntax: wxString wxDOMHTMLElement::GetClassName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLElement::GetClassName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->GetClassName(ns);
        return ns2wx(ns);
    }
    
    return wxEmptyString;
}

// (METHOD) wxDOMHTMLElement::SetClassName
// Description:
//
// Syntax: void wxDOMHTMLElement::SetClassName(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLElement::SetClassName(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    ns_smartptr<nsIDOMHTMLElement> e = m_data->element_ptr;
    
    if (e)
    {
        e->SetClassName(nsvalue);
    }
}

// (METHOD) wxDOMHTMLElement::SetValue
// Description:
//
// Syntax: void wxDOMHTMLElement::SetValue(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLElement::SetValue(const wxString& value)
{
    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    ns_smartptr<nsIDOMHTMLInputElement> e1 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLTextAreaElement> e2 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLSelectElement> e3 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLButtonElement> e4 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLOptionElement> e5 = m_data->element_ptr;
    
    if (e1)
    {
        e1->SetValue(nsvalue);
        return;
    }
     
    if (e2)
    {
        e2->SetValue(nsvalue);
        return;
    }
      
    if (e3)
    {
        e3->SetValue(nsvalue);
        return;
    }
    
    if (e4)
    {
        e4->SetValue(nsvalue);
        return;
    }
    
    if (e5)
    {
        e5->SetValue(nsvalue);
        return;
    }
    
    wxDOMElement e6 = *this;
    e6.SetAttribute(wxT("value"), value);
}

// (METHOD) wxDOMHTMLElement::GetValue
// Description:
//
// Syntax: wxString wxDOMHTMLElement::GetValue()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLElement::GetValue()
{
    ns_smartptr<nsIDOMHTMLInputElement> e1 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLTextAreaElement> e2 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLSelectElement> e3 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLButtonElement> e4 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLOptionElement> e5 = m_data->element_ptr;
    
    nsEmbedString nsvalue;
    
    if (e1)
    {
        e1->GetValue(nsvalue);
        return ns2wx(nsvalue);
    }
     
    if (e2)
    {
        e2->GetValue(nsvalue);
        return ns2wx(nsvalue);
    }
      
    if (e3)
    {
        e3->GetValue(nsvalue);
        return ns2wx(nsvalue);
    }
    
    if (e4)
    {
        e4->GetValue(nsvalue);
        return ns2wx(nsvalue);
    }
    
    if (e5)
    {
        e5->GetValue(nsvalue);
        return ns2wx(nsvalue);
    }
    
    wxDOMElement e6 = *this;
    return e6.GetAttribute(wxT("value"));
}

// (METHOD) wxDOMHTMLElement::HasValueProperty
// Description:
//
// Syntax: bool wxDOMHTMLElement::HasValueProperty() const
//
// Remarks:
//
// Returns:

bool wxDOMHTMLElement::HasValueProperty() const
{
    if (!IsOk())
        return false;
        
    ns_smartptr<nsIDOMHTMLInputElement> e1 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLTextAreaElement> e2 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLSelectElement> e3 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLButtonElement> e4 = m_data->element_ptr;
    ns_smartptr<nsIDOMHTMLOptionElement> e5 = m_data->element_ptr;

    if (e1) return true;
    if (e2) return true;
    if (e3) return true;
    if (e4) return true;
    if (e5) return true;
    
    return false;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLAnchorElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLAnchorElement::wxDOMHTMLAnchorElement
// Description: Creates a new wxDOMHTMLAnchorElement object.
//
// Syntax: wxDOMHTMLAnchorElement::wxDOMHTMLAnchorElement()
//
// Remarks: Creates a new wxDOMHTMLAnchorElement object.

wxDOMHTMLAnchorElement::wxDOMHTMLAnchorElement()
{
}

wxDOMHTMLAnchorElement::wxDOMHTMLAnchorElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLAnchorElement& wxDOMHTMLAnchorElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLAnchorElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLAnchorElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Anchor Element is valid, and false otherwise.

bool wxDOMHTMLAnchorElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    if (!m_data->htmlelement_ptr)
        return false;

    return (m_data->anchor_ptr ? true : false);
}

// (METHOD) wxDOMHTMLAnchorElement::GetAccessKey
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetAccessKey()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetAccessKey()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetAccessKey(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetAccessKey
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetAccessKey(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetAccessKey(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetAccessKey(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetCharset
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetCharset()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetCharset()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetCharset(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetCharset
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetCharset(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetCharset(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetCharset(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetCoords
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetCoords()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetCoords()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetCoords(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetCoords
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetCoords(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetCoords(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetCoords(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetHref
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetHref()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetHref()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetHref(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetHref
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetHref(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetHref(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetHref(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetHreflang
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetHreflang()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetHreflang()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetHreflang(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetHreflang
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetHreflang(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetHreflang(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetHreflang(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetName
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetName(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetName
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetName(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetName(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetName(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetRel
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetRel()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetRel()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetRel(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetRel
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetRel(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetRel(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetRel(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetRev
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetRev()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetRev()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetRev(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetRev
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetRev(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetRev(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetRev(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetShape
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetShape()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetShape()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetShape(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetShape
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetShape(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetShape(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetShape(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetTabIndex
// Description:
//
// Syntax: int wxDOMHTMLAnchorElement::GetTabIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLAnchorElement::GetTabIndex()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->anchor_ptr->GetTabIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLAnchorElement::SetTabIndex
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetTabIndex(int index)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetTabIndex(int index)
{
    if (!IsOk())
        return;
        
    PRInt32 val = index;
    m_data->anchor_ptr->SetTabIndex(val);
}

// (METHOD) wxDOMHTMLAnchorElement::GetTarget
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetTarget()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetTarget()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetTarget(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetTarget
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetTarget(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetTarget(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetTarget(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLAnchorElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLAnchorElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->anchor_ptr->GetType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLAnchorElement::SetType
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::SetType(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::SetType(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->anchor_ptr->SetType(nsvalue);
}

// (METHOD) wxDOMHTMLAnchorElement::Blur
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::Blur()
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::Blur()
{
    if (!IsOk())
        return;
        
    m_data->anchor_ptr->Blur();
}

// (METHOD) wxDOMHTMLAnchorElement::Focus
// Description:
//
// Syntax: void wxDOMHTMLAnchorElement::Focus()
//
// Remarks:
//
// Returns:

void wxDOMHTMLAnchorElement::Focus()
{
    if (!IsOk())
        return;
        
    m_data->anchor_ptr->Focus();
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLButtonElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLButtonElement::wxDOMHTMLButtonElement
// Description: Creates a new wxDOMHTMLButtonElement object.
//
// Syntax: wxDOMHTMLButtonElement::wxDOMHTMLButtonElement()
//
// Remarks: Creates a new wxDOMHTMLButtonElement object.

wxDOMHTMLButtonElement::wxDOMHTMLButtonElement()
{
}

wxDOMHTMLButtonElement::wxDOMHTMLButtonElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLButtonElement& wxDOMHTMLButtonElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLButtonElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLButtonElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Button Element is valid, and false otherwise.

bool wxDOMHTMLButtonElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;

    if (!m_data->htmlelement_ptr)
        return false;

    return (m_data->button_ptr ? true : false);
}

// (METHOD) wxDOMHTMLButtonElement::GetAccessKey
// Description:
//
// Syntax: wxString wxDOMHTMLButtonElement::GetAccessKey()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLButtonElement::GetAccessKey()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->button_ptr->GetAccessKey(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLButtonElement::SetAccessKey
// Description:
//
// Syntax: void wxDOMHTMLButtonElement::SetAccessKey(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLButtonElement::SetAccessKey(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->button_ptr->SetAccessKey(nsvalue);
}

// (METHOD) wxDOMHTMLButtonElement::GetDisabled
// Description:
//
// Syntax: bool wxDOMHTMLButtonElement::GetDisabled()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLButtonElement::GetDisabled()
{
    if (!IsOk())
        return false;

    PRBool b = PR_FALSE;
    m_data->button_ptr->GetDisabled(&b);

    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLButtonElement::SetDisabled
// Description:
//
// Syntax: void wxDOMHTMLButtonElement::SetDisabled(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLButtonElement::SetDisabled(bool value)
{
    if (!IsOk())
        return;

    m_data->button_ptr->SetDisabled(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLButtonElement::GetName
// Description:
//
// Syntax: wxString wxDOMHTMLButtonElement::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLButtonElement::GetName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->button_ptr->GetName(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLButtonElement::SetName
// Description:
//
// Syntax: void wxDOMHTMLButtonElement::SetName(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLButtonElement::SetName(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->button_ptr->SetName(nsvalue);
}

// (METHOD) wxDOMHTMLButtonElement::GetTabIndex
// Description:
//
// Syntax: int wxDOMHTMLButtonElement::GetTabIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLButtonElement::GetTabIndex()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->button_ptr->GetTabIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLButtonElement::SetTabIndex
// Description:
//
// Syntax: void wxDOMHTMLButtonElement::SetTabIndex(int index)
//
// Remarks:
//
// Returns:

void wxDOMHTMLButtonElement::SetTabIndex(int index)
{
    if (!IsOk())
        return;
        
    PRInt32 val = index;
    m_data->button_ptr->SetTabIndex(val);
}

// (METHOD) wxDOMHTMLButtonElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLButtonElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLButtonElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->button_ptr->GetType(ns);

    return ns2wx(ns);
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLInputElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLInputElement::wxDOMHTMLInputElement
// Description: Creates a new wxDOMHTMLInputElement object.
//
// Syntax: wxDOMHTMLInputElement::wxDOMHTMLInputElement()
//
// Remarks: Creates a new wxDOMHTMLInputElement object.

wxDOMHTMLInputElement::wxDOMHTMLInputElement() : wxDOMHTMLElement()
{
}

wxDOMHTMLInputElement::wxDOMHTMLInputElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLInputElement& wxDOMHTMLInputElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLInputElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLInputElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Input Element is valid, and false otherwise.

bool wxDOMHTMLInputElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    if (!m_data->htmlelement_ptr)
        return false;

    return (m_data->input_ptr ? true : false);
}

// (METHOD) wxDOMHTMLInputElement::GetDefaultValue
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetDefaultValue()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetDefaultValue()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetDefaultValue(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetDefaultValue
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetDefaultValue(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetDefaultValue(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetDefaultValue(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetDefaultChecked
// Description:
//
// Syntax: bool wxDOMHTMLInputElement::GetDefaultChecked()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLInputElement::GetDefaultChecked()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->input_ptr->GetDefaultChecked(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLInputElement::SetDefaultChecked
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetDefaultChecked(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetDefaultChecked(bool value)
{
    if (!IsOk())
        return;

    m_data->input_ptr->SetDefaultChecked(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLInputElement::GetAccept
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetAccept()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetAccept()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetAccept(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetAccept
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetAccept(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetAccept(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetAccept(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetAccessKey
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetAccessKey()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetAccessKey()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetAccessKey(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetAccessKey
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetAccessKey(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetAccessKey(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetAccessKey(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetAlign
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetAlign()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetAlign()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetAlign(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetAlign
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetAlign(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetAlign(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetAlign(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetAlt
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetAlt()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetAlt()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetAlt(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetAlt
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetAlt(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetAlt(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetAlt(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetChecked
// Description:
//
// Syntax: bool wxDOMHTMLInputElement::GetChecked()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLInputElement::GetChecked()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->input_ptr->GetChecked(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLInputElement::SetChecked
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetChecked(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetChecked(bool value)
{
    if (!IsOk())
        return;

    m_data->input_ptr->SetChecked(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLInputElement::GetDisabled
// Description:
//
// Syntax: bool wxDOMHTMLInputElement::GetDisabled()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLInputElement::GetDisabled()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->input_ptr->GetDisabled(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLInputElement::SetDisabled
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetDisabled(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetDisabled(bool value)
{
    if (!IsOk())
        return;

    m_data->input_ptr->SetDisabled(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLInputElement::GetMaxLength
// Description:
//
// Syntax: int wxDOMHTMLInputElement::GetMaxLength()
//
// Remarks:
//
// Returns:

int wxDOMHTMLInputElement::GetMaxLength()
{
    if (!IsOk())
        return 0;
        
    PRInt32 val = 0;
    m_data->input_ptr->GetMaxLength(&val);
    return val;
}

// (METHOD) wxDOMHTMLInputElement::SetMaxLength
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetMaxLength(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetMaxLength(int value)
{
    if (!IsOk())
        return;
        
    PRInt32 val = value;
    m_data->input_ptr->SetMaxLength(val);
}

// (METHOD) wxDOMHTMLInputElement::GetName
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetName(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetName
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetName(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetName(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetName(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetReadOnly
// Description:
//
// Syntax: bool wxDOMHTMLInputElement::GetReadOnly()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLInputElement::GetReadOnly()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->input_ptr->GetReadOnly(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLInputElement::SetReadOnly
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetReadOnly(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetReadOnly(bool value)
{
    if (!IsOk())
        return;

    m_data->input_ptr->SetReadOnly(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLInputElement::GetSize
// Description:
//
// Syntax: int wxDOMHTMLInputElement::GetSize()
//
// Remarks:
//
// Returns:

int wxDOMHTMLInputElement::GetSize()
{
    if (!IsOk())
        return 0;
        
    PRUint32 val = 0;
    m_data->input_ptr->GetSize(&val);
    return val;
}

// (METHOD) wxDOMHTMLInputElement::SetSize
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetSize(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetSize(int value)
{
    if (!IsOk())
        return;
        
    PRUint32 val = value;
    m_data->input_ptr->SetSize(val);
}

// (METHOD) wxDOMHTMLInputElement::GetSrc
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetSrc()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetSrc()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetSrc(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetSrc
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetSrc(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetSrc(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetSrc(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetTabIndex
// Description:
//
// Syntax: int wxDOMHTMLInputElement::GetTabIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLInputElement::GetTabIndex()
{
    if (!IsOk())
        return 0;
        
    PRInt32 val = 0;
    m_data->input_ptr->GetTabIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLInputElement::SetTabIndex
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetTabIndex(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetTabIndex(int value)
{
    if (!IsOk())
        return;
        
    PRInt32 val = value;
    m_data->input_ptr->SetTabIndex(val);
}

// (METHOD) wxDOMHTMLInputElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetType
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetType(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetType(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetType(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::GetUseMap
// Description:
//
// Syntax: wxString wxDOMHTMLInputElement::GetUseMap()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLInputElement::GetUseMap()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->input_ptr->GetUseMap(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLInputElement::SetUseMap
// Description:
//
// Syntax: void wxDOMHTMLInputElement::SetUseMap(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::SetUseMap(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->input_ptr->SetUseMap(nsvalue);
}

// (METHOD) wxDOMHTMLInputElement::Blur
// Description:
//
// Syntax: void wxDOMHTMLInputElement::Blur()
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::Blur()
{
    if (!IsOk())
        return;
        
    m_data->input_ptr->Blur();
}

// (METHOD) wxDOMHTMLInputElement::Focus
// Description:
//
// Syntax: void wxDOMHTMLInputElement::Focus()
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::Focus()
{
    if (!IsOk())
        return;
        
    m_data->input_ptr->Focus();
}

// (METHOD) wxDOMHTMLInputElement::Select
// Description:
//
// Syntax: void wxDOMHTMLInputElement::Select()
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::Select()
{
    if (!IsOk())
        return;
        
    m_data->input_ptr->Select();
}

// (METHOD) wxDOMHTMLInputElement::Click
// Description:
//
// Syntax: void wxDOMHTMLInputElement::Click()
//
// Remarks:
//
// Returns:

void wxDOMHTMLInputElement::Click()
{
    if (!IsOk())
        return;
        
    m_data->input_ptr->Click();
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLLinkElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLLinkElement::wxDOMHTMLLinkElement
// Description: Creates a new wxDOMHTMLLinkElement object.
//
// Syntax: wxDOMHTMLLinkElement::wxDOMHTMLLinkElement()
//
// Remarks: Creates a new wxDOMHTMLLinkElement object.

wxDOMHTMLLinkElement::wxDOMHTMLLinkElement()
{
}

wxDOMHTMLLinkElement::wxDOMHTMLLinkElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLLinkElement& wxDOMHTMLLinkElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLLinkElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLLinkElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Link Element is valid, and false otherwise.

bool wxDOMHTMLLinkElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
        
    if (!m_data->htmlelement_ptr)
        return false;

    return (m_data->link_ptr ? true : false);
}

// (METHOD) wxDOMHTMLLinkElement::GetDisabled
// Description:
//
// Syntax: bool wxDOMHTMLLinkElement::GetDisabled()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLLinkElement::GetDisabled()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->link_ptr->GetDisabled(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLLinkElement::SetDisabled
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetDisabled(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetDisabled(bool value)
{
    if (!IsOk())
        return;

    m_data->link_ptr->SetDisabled(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLLinkElement::GetCharset
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetCharset()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetCharset()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetCharset(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetCharset
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetCharset(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetCharset(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetCharset(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetHref
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetHref()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetHref()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetHref(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetHref
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetHref(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetHref(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetHref(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetHreflang
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetHreflang()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetHreflang()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetHreflang(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetHreflang
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetHreflang(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetHreflang(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetHreflang(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetMedia
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetMedia()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetMedia()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetMedia(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetMedia
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetMedia(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetMedia(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetMedia(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetRel
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetRel()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetRel()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetRel(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetRel
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetRel(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetRel(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetRel(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetRev
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetRev()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetRev()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetRev(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetRev
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetRev(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetRev(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetRev(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetTarget
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetTarget()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetTarget()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetTarget(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetTarget
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetTarget(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetTarget(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetTarget(nsvalue);
}

// (METHOD) wxDOMHTMLLinkElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLLinkElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLLinkElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->link_ptr->GetType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLLinkElement::SetType
// Description:
//
// Syntax: void wxDOMHTMLLinkElement::SetType(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLLinkElement::SetType(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);

    m_data->link_ptr->SetType(nsvalue);
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLOptionElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLOptionElement::wxDOMHTMLOptionElement
// Description: Creates a new wxDOMHTMLOptionElement object.
//
// Syntax: wxDOMHTMLOptionElement::wxDOMHTMLOptionElement()
//
// Remarks: Creates a new wxDOMHTMLOptionElement object.

wxDOMHTMLOptionElement::wxDOMHTMLOptionElement() : wxDOMHTMLElement()
{
}

wxDOMHTMLOptionElement::wxDOMHTMLOptionElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLOptionElement& wxDOMHTMLOptionElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLOptionElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLOptionElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Option Element is valid, and false otherwise.

bool wxDOMHTMLOptionElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
    
    if (!m_data->htmlelement_ptr)
        return false;
    
    return (m_data->option_ptr ? true : false);
}

// (METHOD) wxDOMHTMLOptionElement::GetDefaultSelected
// Description:
//
// Syntax: bool wxDOMHTMLOptionElement::GetDefaultSelected()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLOptionElement::GetDefaultSelected()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->option_ptr->GetDefaultSelected(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLOptionElement::SetDefaultSelected
// Description:
//
// Syntax: void wxDOMHTMLOptionElement::SetDefaultSelected(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLOptionElement::SetDefaultSelected(bool value)
{
    if (!IsOk())
        return;

    m_data->option_ptr->SetDefaultSelected(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLOptionElement::GetText
// Description:
//
// Syntax: wxString wxDOMHTMLOptionElement::GetText()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLOptionElement::GetText()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->option_ptr->GetText(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLOptionElement::GetIndex
// Description:
//
// Syntax: int wxDOMHTMLOptionElement::GetIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLOptionElement::GetIndex()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->option_ptr->GetIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLOptionElement::GetDisabled
// Description:
//
// Syntax: bool wxDOMHTMLOptionElement::GetDisabled()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLOptionElement::GetDisabled()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->option_ptr->GetDisabled(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLOptionElement::SetDisabled
// Description:
//
// Syntax: void wxDOMHTMLOptionElement::SetDisabled(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLOptionElement::SetDisabled(bool value)
{
    if (!IsOk())
        return;

    m_data->option_ptr->SetDisabled(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLOptionElement::GetLabel
// Description:
//
// Syntax: wxString wxDOMHTMLOptionElement::GetLabel()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLOptionElement::GetLabel()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->option_ptr->GetLabel(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLOptionElement::SetLabel
// Description:
//
// Syntax: void wxDOMHTMLOptionElement::SetLabel(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLOptionElement::SetLabel(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->option_ptr->SetLabel(nsvalue);
}

// (METHOD) wxDOMHTMLOptionElement::GetSelected
// Description:
//
// Syntax: bool wxDOMHTMLOptionElement::GetSelected()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLOptionElement::GetSelected()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->option_ptr->GetSelected(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLOptionElement::SetSelected
// Description:
//
// Syntax: void wxDOMHTMLOptionElement::SetSelected(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLOptionElement::SetSelected(bool value)
{
    if (!IsOk())
        return;

    m_data->option_ptr->SetSelected(value ? PR_TRUE : PR_FALSE);
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLParamElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLParamElement::wxDOMHTMLParamElement
// Description: Creates a new wxDOMHTMLParamElement object.
//
// Syntax: wxDOMHTMLParamElement::wxDOMHTMLParamElement()
//
// Remarks: Creates a new wxDOMHTMLParamElement object.

wxDOMHTMLParamElement::wxDOMHTMLParamElement()
{
}

wxDOMHTMLParamElement::wxDOMHTMLParamElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLParamElement& wxDOMHTMLParamElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLParamElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLParamElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Param Element is valid, and false otherwise.

bool wxDOMHTMLParamElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
    
    if (!m_data->htmlelement_ptr)
        return false;
    
    return (m_data->param_ptr ? true : false);
}

// (METHOD) wxDOMHTMLParamElement::GetName
// Description:
//
// Syntax: wxString wxDOMHTMLParamElement::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLParamElement::GetName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->param_ptr->GetName(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLParamElement::SetName
// Description:
//
// Syntax: void wxDOMHTMLParamElement::SetName(const wxString& name)
//
// Remarks:
//
// Returns:

void wxDOMHTMLParamElement::SetName(const wxString& name)
{
    if (!IsOk())
        return;

    nsEmbedString nsname;
    wx2ns(name, nsname);
    
    m_data->param_ptr->SetName(nsname);
}

// (METHOD) wxDOMHTMLParamElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLParamElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLParamElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->param_ptr->GetType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLParamElement::SetType
// Description:
//
// Syntax: void wxDOMHTMLParamElement::SetType(const wxString& type)
//
// Remarks:
//
// Returns:

void wxDOMHTMLParamElement::SetType(const wxString& type)
{
    if (!IsOk())
        return;

    nsEmbedString nstype;
    wx2ns(type, nstype);
    
    m_data->param_ptr->SetType(nstype);
}

// (METHOD) wxDOMHTMLParamElement::GetValueType
// Description:
//
// Syntax: wxString wxDOMHTMLParamElement::GetValueType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLParamElement::GetValueType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->param_ptr->GetValueType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLParamElement::SetValueType
// Description:
//
// Syntax: void wxDOMHTMLParamElement::SetValueType(const wxString& valuetype)
//
// Remarks:
//
// Returns:

void wxDOMHTMLParamElement::SetValueType(const wxString& valuetype)
{
    if (!IsOk())
        return;

    nsEmbedString nsvaluetype;
    wx2ns(valuetype, nsvaluetype);
    
    m_data->param_ptr->SetValueType(nsvaluetype);
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLSelectElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLSelectElement::wxDOMHTMLSelectElement
// Description: Creates a new wxDOMHTMLSelectElement object.
//
// Syntax: wxDOMHTMLSelectElement::wxDOMHTMLSelectElement()
//
// Remarks: Creates a new wxDOMHTMLSelectElement object.

wxDOMHTMLSelectElement::wxDOMHTMLSelectElement() : wxDOMHTMLElement()
{
}

wxDOMHTMLSelectElement::wxDOMHTMLSelectElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLSelectElement& wxDOMHTMLSelectElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLSelectElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLSelectElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Select Element is valid, and false otherwise.

bool wxDOMHTMLSelectElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
    
    if (!m_data->htmlelement_ptr)
        return false;
    
    return (m_data->select_ptr ? true : false);
}

// (METHOD) wxDOMHTMLSelectElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLSelectElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLSelectElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->select_ptr->GetType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLSelectElement::GetSelectedIndex
// Description:
//
// Syntax: int wxDOMHTMLSelectElement::GetSelectedIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLSelectElement::GetSelectedIndex()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->select_ptr->GetSelectedIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLSelectElement::SetSelectedIndex
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetSelectedIndex(int index)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetSelectedIndex(int index)
{
    if (!IsOk())
        return;
        
    PRInt32 val = index;
    m_data->select_ptr->SetSelectedIndex(val);
}

// (METHOD) wxDOMHTMLSelectElement::GetLength
// Description:
//
// Syntax: int wxDOMHTMLSelectElement::GetLength()
//
// Remarks:
//
// Returns:

int wxDOMHTMLSelectElement::GetLength()
{
    if (!IsOk())
        return 0;

    PRUint32 val = 0;
    m_data->select_ptr->GetLength(&val);
    return val;
}

// (METHOD) wxDOMHTMLSelectElement::SetLength
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetLength(int length)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetLength(int length)
{
    if (!IsOk())
        return;
        
    PRUint32 val = length;
    m_data->select_ptr->SetLength(val);
}

// (METHOD) wxDOMHTMLSelectElement::GetDisabled
// Description:
//
// Syntax: bool wxDOMHTMLSelectElement::GetDisabled()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLSelectElement::GetDisabled()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->select_ptr->GetDisabled(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLSelectElement::SetDisabled
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetDisabled(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetDisabled(bool value)
{
    if (!IsOk())
        return;

    m_data->select_ptr->SetDisabled(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLSelectElement::GetMultiple
// Description:
//
// Syntax: bool wxDOMHTMLSelectElement::GetMultiple()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLSelectElement::GetMultiple()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->select_ptr->GetMultiple(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLSelectElement::SetMultiple
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetMultiple(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetMultiple(bool value)
{
    if (!IsOk())
        return;

    m_data->select_ptr->SetMultiple(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLSelectElement::GetName
// Description:
//
// Syntax: wxString wxDOMHTMLSelectElement::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLSelectElement::GetName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->select_ptr->GetName(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLSelectElement::SetName
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetName(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetName(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->select_ptr->SetName(nsvalue);
}

// (METHOD) wxDOMHTMLSelectElement::GetSize
// Description:
//
// Syntax: int wxDOMHTMLSelectElement::GetSize()
//
// Remarks:
//
// Returns:

int wxDOMHTMLSelectElement::GetSize()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->select_ptr->GetSize(&val);
    return val;
}

// (METHOD) wxDOMHTMLSelectElement::SetSize
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetSize(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetSize(int value)
{
    if (!IsOk())
        return;
        
    PRInt32 val = value;
    m_data->select_ptr->SetSize(val);
}

// (METHOD) wxDOMHTMLSelectElement::GetTabIndex
// Description:
//
// Syntax: int wxDOMHTMLSelectElement::GetTabIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLSelectElement::GetTabIndex()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->select_ptr->GetTabIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLSelectElement::SetTabIndex
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::SetTabIndex(int index)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::SetTabIndex(int index)
{
    if (!IsOk())
        return;
        
    PRInt32 val = index;
    m_data->select_ptr->SetTabIndex(val);
}

// (METHOD) wxDOMHTMLSelectElement::Add
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::Add(const wxDOMHTMLElement& element,
//                                          const wxDOMHTMLElement& before)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::Add(const wxDOMHTMLElement& element,
                                 const wxDOMHTMLElement& before)
{
    if (!IsOk())
        return;
        
    m_data->select_ptr->Add(element.m_data->htmlelement_ptr,
                            before.m_data->htmlelement_ptr);
}

// (METHOD) wxDOMHTMLSelectElement::Remove
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::Remove(int index)
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::Remove(int index)
{
    if (!IsOk())
        return;

    m_data->select_ptr->Remove(index);
}

// (METHOD) wxDOMHTMLSelectElement::Blur
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::Blur()
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::Blur()
{
    if (!IsOk())
        return;

    m_data->select_ptr->Blur();
}

// (METHOD) wxDOMHTMLSelectElement::Focus
// Description:
//
// Syntax: void wxDOMHTMLSelectElement::Focus()
//
// Remarks:
//
// Returns:

void wxDOMHTMLSelectElement::Focus()
{
    if (!IsOk())
        return;

    m_data->select_ptr->Focus();
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLTextAreaElement class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMHTMLTextAreaElement::wxDOMHTMLTextAreaElement
// Description: Creates a new wxDOMHTMLTextAreaElement object.
//
// Syntax: wxDOMHTMLTextAreaElement::wxDOMHTMLTextAreaElement()
//
// Remarks: Creates a new wxDOMHTMLTextAreaElement object.

wxDOMHTMLTextAreaElement::wxDOMHTMLTextAreaElement() : wxDOMHTMLElement()
{
}

wxDOMHTMLTextAreaElement::wxDOMHTMLTextAreaElement(const wxDOMNode& node) : wxDOMHTMLElement(node)
{
}

wxDOMHTMLTextAreaElement& wxDOMHTMLTextAreaElement::operator=(const wxDOMNode& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMHTMLTextAreaElement::IsOk
// Description:
//
// Syntax: bool wxDOMHTMLTextAreaElement::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM HTML Text Area Element is valid, and false otherwise.

bool wxDOMHTMLTextAreaElement::IsOk() const
{
    if (!m_data->node_ptr)
        return false;
    
    if (!m_data->htmlelement_ptr)
        return false;
    
    return (m_data->textarea_ptr ? true : false);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetDefaultValue
// Description:
//
// Syntax: wxString wxDOMHTMLTextAreaElement::GetDefaultValue()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLTextAreaElement::GetDefaultValue()
{
    if (!IsOk())
        return wxEmptyString;
    
    nsEmbedString ns;
    m_data->textarea_ptr->GetDefaultValue(ns);
    
    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLTextAreaElement::SetDefaultValue
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetDefaultValue(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetDefaultValue(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->textarea_ptr->SetDefaultValue(nsvalue);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetAccessKey
// Description:
//
// Syntax: wxString wxDOMHTMLTextAreaElement::GetAccessKey()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLTextAreaElement::GetAccessKey()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->textarea_ptr->GetAccessKey(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLTextAreaElement::SetAccessKey
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetAccessKey(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetAccessKey(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->textarea_ptr->SetAccessKey(nsvalue);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetCols
// Description:
//
// Syntax: int wxDOMHTMLTextAreaElement::GetCols()
//
// Remarks:
//
// Returns:

int wxDOMHTMLTextAreaElement::GetCols()
{
    if (!IsOk())
        return 0;
        
    PRInt32 val = 0;
    m_data->textarea_ptr->GetCols(&val);
    return val;
}

// (METHOD) wxDOMHTMLTextAreaElement::SetCols
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetCols(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetCols(int value)
{
    if (!IsOk())
        return;
        
    PRInt32 val = value;
    m_data->textarea_ptr->SetCols(val);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetDisabled
// Description:
//
// Syntax: bool wxDOMHTMLTextAreaElement::GetDisabled()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLTextAreaElement::GetDisabled()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->textarea_ptr->GetDisabled(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLTextAreaElement::SetDisabled
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetDisabled(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetDisabled(bool value)
{
    if (!IsOk())
        return;

    m_data->textarea_ptr->SetDisabled(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetName
// Description:
//
// Syntax: wxString wxDOMHTMLTextAreaElement::GetName()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLTextAreaElement::GetName()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->textarea_ptr->GetName(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLTextAreaElement::SetName
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetName(const wxString& value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetName(const wxString& value)
{
    if (!IsOk())
        return;

    nsEmbedString nsvalue;
    wx2ns(value, nsvalue);
    
    m_data->textarea_ptr->SetName(nsvalue);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetReadOnly
// Description:
//
// Syntax: bool wxDOMHTMLTextAreaElement::GetReadOnly()
//
// Remarks:
//
// Returns:

bool wxDOMHTMLTextAreaElement::GetReadOnly()
{
    if (!IsOk())
        return false;
        
    PRBool b = PR_FALSE;
    m_data->textarea_ptr->GetReadOnly(&b);
    
    return (b == PR_TRUE ? true : false);
}

// (METHOD) wxDOMHTMLTextAreaElement::SetReadOnly
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetReadOnly(bool value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetReadOnly(bool value)
{
    if (!IsOk())
        return;

    m_data->textarea_ptr->SetReadOnly(value ? PR_TRUE : PR_FALSE);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetRows
// Description:
//
// Syntax: int wxDOMHTMLTextAreaElement::GetRows()
//
// Remarks:
//
// Returns:

int wxDOMHTMLTextAreaElement::GetRows()
{
    if (!IsOk())
        return 0;
        
    PRInt32 val = 0;
    m_data->textarea_ptr->GetRows(&val);
    return val;
}

// (METHOD) wxDOMHTMLTextAreaElement::SetRows
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetRows(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetRows(int value)
{
    if (!IsOk())
        return;
        
    PRInt32 val = value;
    m_data->textarea_ptr->SetRows(val);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetTabIndex
// Description:
//
// Syntax: int wxDOMHTMLTextAreaElement::GetTabIndex()
//
// Remarks:
//
// Returns:

int wxDOMHTMLTextAreaElement::GetTabIndex()
{
    if (!IsOk())
        return 0;

    PRInt32 val = 0;
    m_data->textarea_ptr->GetTabIndex(&val);
    return val;
}

// (METHOD) wxDOMHTMLTextAreaElement::SetTabIndex
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::SetTabIndex(int value)
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::SetTabIndex(int value)
{
    if (!IsOk())
        return;
        
    PRInt32 val = value;
    m_data->textarea_ptr->SetTabIndex(val);
}

// (METHOD) wxDOMHTMLTextAreaElement::GetType
// Description:
//
// Syntax: wxString wxDOMHTMLTextAreaElement::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMHTMLTextAreaElement::GetType()
{
    if (!IsOk())
        return wxEmptyString;

    nsEmbedString ns;
    m_data->textarea_ptr->GetType(ns);

    return ns2wx(ns);
}

// (METHOD) wxDOMHTMLTextAreaElement::Blur
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::Blur()
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::Blur()
{
    if (!IsOk())
        return;
        
    m_data->textarea_ptr->Blur();
}

// (METHOD) wxDOMHTMLTextAreaElement::Focus
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::Focus()
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::Focus()
{
    if (!IsOk())
        return;
        
    m_data->textarea_ptr->Focus();
}

// (METHOD) wxDOMHTMLTextAreaElement::Select
// Description:
//
// Syntax: void wxDOMHTMLTextAreaElement::Select()
//
// Remarks:
//
// Returns:

void wxDOMHTMLTextAreaElement::Select()
{
    if (!IsOk())
        return;
        
    m_data->textarea_ptr->Select();
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMNodeList class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMNodeList::wxDOMNodeList
// Description: Creates a new wxDOMNodeList object.
//
// Syntax: wxDOMNodeList::wxDOMNodeList()
//
// Remarks: Creates a new wxDOMNodeList object.

wxDOMNodeList::wxDOMNodeList()
{
    m_data = new wxDOMNodeListData;
}

wxDOMNodeList::~wxDOMNodeList()
{
    delete m_data;
}

wxDOMNodeList::wxDOMNodeList(const wxDOMNodeList& c)
{
    m_data = new wxDOMNodeListData;
    m_data->ptr = c.m_data->ptr;
}

wxDOMNodeList& wxDOMNodeList::operator=(const wxDOMNodeList& c)
{
    m_data->ptr = c.m_data->ptr;
    return *this;
}

// (METHOD) wxDOMNodeList::IsOk
// Description:
//
// Syntax: bool wxDOMNodeList::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Node List is valid, and false otherwise.

bool wxDOMNodeList::IsOk() const
{
    return (m_data->ptr ? true : false);
}

// (METHOD) wxDOMNodeList::Item
// Description:
//
// Syntax: wxDOMNode wxDOMNodeList::Item(size_t idx)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNodeList::Item(size_t idx)
{
    wxDOMNode node;
    if (!IsOk())
        return node;
    m_data->ptr->Item(idx, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNodeList::GetLength
// Description:
//
// Syntax: size_t wxDOMNodeList::GetLength()
//
// Remarks:
//
// Returns:

size_t wxDOMNodeList::GetLength()
{
    if (!IsOk())
        return 0;
        
    PRUint32 res;
    m_data->ptr->GetLength(&res);
    return res;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMNamedNodeMap class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMNamedNodeMap::wxDOMNamedNodeMap
// Description: Creates a new wxDOMNamedNodeMap object.
//
// Syntax: wxDOMNamedNodeMap::wxDOMNamedNodeMap()
//
// Remarks: Creates a new wxDOMNamedNodeMap object.

wxDOMNamedNodeMap::wxDOMNamedNodeMap()
{
    m_data = new wxDOMNamedNodeMapData;
}

wxDOMNamedNodeMap::~wxDOMNamedNodeMap()
{
    delete m_data;
}

wxDOMNamedNodeMap::wxDOMNamedNodeMap(const wxDOMNamedNodeMap& c)
{
    m_data = new wxDOMNamedNodeMapData;
    m_data->ptr = c.m_data->ptr;
}

wxDOMNamedNodeMap& wxDOMNamedNodeMap::operator=(const wxDOMNamedNodeMap& c)
{
    m_data->ptr = c.m_data->ptr;
    return *this;
}

// (METHOD) wxDOMNamedNodeMap::IsOk
// Description:
//
// Syntax: bool wxDOMNamedNodeMap::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Named Node Map is valid, and false otherwise.

bool wxDOMNamedNodeMap::IsOk() const
{
    return (m_data->ptr ? true : false);
}

// (METHOD) wxDOMNamedNodeMap::GetLength
// Description:
//
// Syntax: size_t wxDOMNamedNodeMap::GetLength()
//
// Remarks:
//
// Returns:

size_t wxDOMNamedNodeMap::GetLength()
{
    if (!IsOk())
        return 0;
    PRUint32 res;
    m_data->ptr->GetLength(&res);
    return res;
}

// (METHOD) wxDOMNamedNodeMap::Item
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::Item(size_t idx)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::Item(size_t idx)
{
    wxDOMNode node;
    if (!IsOk())
        return node;

    m_data->ptr->Item(idx, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNamedNodeMap::GetNamedItem
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::GetNamedItem(const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::GetNamedItem(const wxString& name)
{
    nsEmbedString nsstr;
    wx2ns(name, nsstr);
    
    wxDOMNode node;
    if (!IsOk())
        return node;

    m_data->ptr->GetNamedItem(nsstr, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNamedNodeMap::GetNamedItemNS
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::GetNamedItemNS(const wxString& namespace_uri,
//                                                     const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::GetNamedItemNS(const wxString& namespace_uri,
                                            const wxString& name)
{
    nsEmbedString nsstr_uri, nsstr_name;
    wx2ns(namespace_uri, nsstr_uri);
    wx2ns(name, nsstr_name);
    
    wxDOMNode node;
    if (!IsOk())
        return node;
    m_data->ptr->GetNamedItemNS(nsstr_uri, nsstr_name, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNamedNodeMap::RemoveNamedItem
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::RemoveNamedItem(const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::RemoveNamedItem(const wxString& name)
{
    nsEmbedString nsstr;
    wx2ns(name, nsstr);
    
    wxDOMNode node;
    if (!IsOk())
        return node;
    m_data->ptr->RemoveNamedItem(nsstr, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNamedNodeMap::RemoveNamedItemNS
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::RemoveNamedItemNS(const wxString& namespace_uri,
//                                                        const wxString& name)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::RemoveNamedItemNS(const wxString& namespace_uri,
                                               const wxString& name)
{
    nsEmbedString nsstr_uri, nsstr_name;
    wx2ns(namespace_uri, nsstr_uri);
    wx2ns(name, nsstr_name);
        
    wxDOMNode node;
    if (!IsOk())
        return node;
    m_data->ptr->RemoveNamedItemNS(nsstr_uri, nsstr_name, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNamedNodeMap::SetNamedItem
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::SetNamedItem(wxDOMNode& arg)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::SetNamedItem(wxDOMNode& arg)
{
    wxDOMNode node;
    if (!IsOk())
        return node;
    m_data->ptr->SetNamedItem(arg.m_data->node_ptr, &node.m_data->node_ptr.p);
    return node;
}

// (METHOD) wxDOMNamedNodeMap::SetNamedItemNS
// Description:
//
// Syntax: wxDOMNode wxDOMNamedNodeMap::SetNamedItemNS(wxDOMNode& arg)
//
// Remarks:
//
// Returns:

wxDOMNode wxDOMNamedNodeMap::SetNamedItemNS(wxDOMNode& arg)
{
    wxDOMNode node;
    if (!IsOk())
        return node;
    m_data->ptr->SetNamedItemNS(arg.m_data->node_ptr, &node.m_data->node_ptr.p);
    return node;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMEvent class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMEvent::wxDOMEvent
// Description: Creates a new wxDOMEvent object.
//
// Syntax: wxDOMEvent::wxDOMEvent()
//
// Remarks: Creates a new wxDOMEvent object.

wxDOMEvent::wxDOMEvent()
{
    m_data = new wxDOMEventData;
}

wxDOMEvent::~wxDOMEvent()
{
    delete m_data;
}

wxDOMEvent::wxDOMEvent(const wxDOMEvent& c)
{
    m_data = new wxDOMEventData;
    assign(c);
}

wxDOMEvent& wxDOMEvent::operator=(const wxDOMEvent& c)
{
    assign(c);
    return *this;
}

void wxDOMEvent::assign(const wxDOMEvent& c)
{
    m_data->setPtr(c.m_data->event_ptr.p);
}

// (METHOD) wxDOMEvent::IsOk
// Description:
//
// Syntax: bool wxDOMEvent::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Event is valid, and false otherwise.

bool wxDOMEvent::IsOk() const
{
    return (m_data->event_ptr ? true : false);
}

// (METHOD) wxDOMEvent::GetType
// Description:
//
// Syntax: wxString wxDOMEvent::GetType()
//
// Remarks:
//
// Returns:

wxString wxDOMEvent::GetType()
{
    if (!m_data)
        return wxEmptyString;
    
    wxString res;
    
    return wxEmptyString;
}




///////////////////////////////////////////////////////////////////////////////
//  wxDOMMouseEvent class implementation
///////////////////////////////////////////////////////////////////////////////


// (CONSTRUCTOR) wxDOMMouseEvent::wxDOMMouseEvent
// Description: Creates a new wxDOMMouseEvent object.
//
// Syntax: wxDOMMouseEvent::wxDOMMouseEvent()
//
// Remarks: Creates a new wxDOMMouseEvent object.

wxDOMMouseEvent::wxDOMMouseEvent() : wxDOMEvent()
{
}

wxDOMMouseEvent::wxDOMMouseEvent(const wxDOMEvent& evt) : wxDOMEvent(evt)
{
}

wxDOMMouseEvent& wxDOMMouseEvent::operator=(const wxDOMEvent& c)
{
    assign(c);
    return *this;
}

// (METHOD) wxDOMMouseEvent::IsOk
// Description:
//
// Syntax: bool wxDOMMouseEvent::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the DOM Mouse Event is valid, and false otherwise.

bool wxDOMMouseEvent::IsOk() const
{
    if (!m_data->mouseevent_ptr)
        return false;
        
    return (m_data->mouseevent_ptr ? true : false);
}

// (METHOD) wxDOMMouseEvent::GetScreenX
// Description:
//
// Syntax: long wxDOMMouseEvent::GetScreenX()
//
// Remarks:
//
// Returns:

long wxDOMMouseEvent::GetScreenX()
{
    // TODO: implement
    return 0;
}

// (METHOD) wxDOMMouseEvent::GetScreenY
// Description:
//
// Syntax: long wxDOMMouseEvent::GetScreenY()
//
// Remarks:
//
// Returns:

long wxDOMMouseEvent::GetScreenY()
{
    // TODO: implement
    return 0;
}

// (METHOD) wxDOMMouseEvent::GetClientX
// Description:
//
// Syntax: long wxDOMMouseEvent::GetClientX()
//
// Remarks:
//
// Returns:

long wxDOMMouseEvent::GetClientX()
{
    // TODO: implement
    return 0;
}

// (METHOD) wxDOMMouseEvent::GetClientY
// Description:
//
// Syntax: long wxDOMMouseEvent::GetClientY()
//
// Remarks:
//
// Returns:

long wxDOMMouseEvent::GetClientY()
{
    // TODO: implement
    return 0;
}

// (METHOD) wxDOMMouseEvent::GetCtrlKey
// Description:
//
// Syntax: bool wxDOMMouseEvent::GetCtrlKey()
//
// Remarks:
//
// Returns:

bool wxDOMMouseEvent::GetCtrlKey()
{
    // TODO: implement
    return false;
}

// (METHOD) wxDOMMouseEvent::GetShiftKey
// Description:
//
// Syntax: bool wxDOMMouseEvent::GetShiftKey()
//
// Remarks:
//
// Returns:

bool wxDOMMouseEvent::GetShiftKey()
{
    // TODO: implement
    return false;
}

// (METHOD) wxDOMMouseEvent::GetAltKey
// Description:
//
// Syntax: bool wxDOMMouseEvent::GetAltKey()
//
// Remarks:
//
// Returns:

bool wxDOMMouseEvent::GetAltKey()
{
    // TODO: implement
    return false;
}

// (METHOD) wxDOMMouseEvent::GetMetaKey
// Description:
//
// Syntax: bool wxDOMMouseEvent::GetMetaKey()
//
// Remarks:
//
// Returns:

bool wxDOMMouseEvent::GetMetaKey()
{
    // TODO: implement
    return false;
}

// (METHOD) wxDOMMouseEvent::GetButton
// Description:
//
// Syntax: unsigned short wxDOMMouseEvent::GetButton()
//
// Remarks:
//
// Returns:

unsigned short wxDOMMouseEvent::GetButton()
{
    // TODO: implement
    return 0;
}

