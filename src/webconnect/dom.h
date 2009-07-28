///////////////////////////////////////////////////////////////////////////////
// Name:        dom.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2009-05-14
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_DOM_H
#define __WXWEBCONNECT_DOM_H


class wxDOMNode;
class wxDOMElement;
class wxDOMDocument;
class wxDOMNodeList;
class wxDOMNamedNodeMap;
struct wxDOMNodeData;
struct wxDOMNodeListData;
struct wxDOMNamedNodeMapData;
struct wxDOMEventData;



///////////////////////////////////////////////////////////////////////////////
//  wxDOMNode class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMNode
// Category: DOM
// Description: Encapsulates a DOM node.
// Remarks: The wxDOMNode class encapsulates a DOM node.

class wxDOMNode
{
friend class BrowserChrome;
friend class wxWebControl;
friend class wxDOMAttr;
friend class wxDOMElement;
friend class wxDOMDocument;
friend class wxDOMNodeList;
friend class wxDOMNamedNodeMap;
friend class wxDOMHTMLSelectElement;

public:

    wxDOMNode();
    ~wxDOMNode();
    wxDOMNode(const wxDOMNode& c);
    wxDOMNode& operator=(const wxDOMNode& c);
    
    void assign(const wxDOMNode& c);
    
    virtual bool IsOk() const;

public:

    // DOMNode interface

    wxDOMDocument GetOwnerDocument();
    
    wxString GetNodeName();
    int GetNodeType();

    wxString GetNodeValue();
    void SetNodeValue(const wxString& value);

    wxDOMNode GetParentNode();
    wxDOMNodeList GetChildNodes();

    wxDOMNode GetFirstChild();
    wxDOMNode GetLastChild();

    wxDOMNode GetPreviousSibling();
    wxDOMNode GetNextSibling();

    wxDOMNode InsertBefore(wxDOMNode& new_child, wxDOMNode& ref_child);
    wxDOMNode ReplaceChild(wxDOMNode& new_child, wxDOMNode& old_child);
    wxDOMNode RemoveChild(wxDOMNode& old_child);
    wxDOMNode AppendChild(wxDOMNode& new_child);
    
    wxDOMNamedNodeMap GetAttributes();

    wxDOMNode CloneNode(bool deep);
    void Normalize();

    bool IsSupported(const wxString& feature, const wxString& version);
    bool HasChildNodes();
    bool HasAttributes();

    wxString GetPrefix();
    void SetPrefix(const wxString& value);

    wxString GetNamespaceURI();
    wxString GetLocalName();
    
public:

    bool AddEventListener(const wxString& type,
                          wxEvtHandler* event_handler,
                          int event_id = -1,
                          bool use_capture = false);

protected:

    wxDOMNodeData* m_data;
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMNodeList class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMNodeList
// Category: DOM
// Description: Encapsulates a DOM node list.
// Remarks: The wxDOMNodeList class encapsulates a DOM node list.

class wxDOMNodeList
{
friend class wxDOMNode;
friend class wxDOMElement;
friend class wxDOMDocument;

public:

    wxDOMNodeList();
    ~wxDOMNodeList();
    wxDOMNodeList(const wxDOMNodeList& c);
    wxDOMNodeList& operator=(const wxDOMNodeList& c);

    bool IsOk() const;

    wxDOMNode Item(size_t idx);
    size_t GetLength();
        
private:

    wxDOMNodeListData* m_data;
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMNamedNodeMap class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMNamedNodeMap
// Category: DOM
// Description: Encapsulates a DOM named node map.
// Remarks: The wxDOMNamedNodeMap encapsulates a DOM named node map.

class wxDOMNamedNodeMap
{
friend class wxDOMNode;

public:

    wxDOMNamedNodeMap();
    ~wxDOMNamedNodeMap();
    wxDOMNamedNodeMap(const wxDOMNamedNodeMap& c);
    wxDOMNamedNodeMap& operator=(const wxDOMNamedNodeMap& c);
    
    bool IsOk() const;
    
    size_t GetLength();
    wxDOMNode Item(size_t idx);
    
    wxDOMNode GetNamedItem(const wxString& name);
    wxDOMNode GetNamedItemNS(const wxString& namespace_uri,
                             const wxString& name);
    
    wxDOMNode RemoveNamedItem(const wxString& name);
    wxDOMNode RemoveNamedItemNS(const wxString& namespace_uri,
                                const wxString& name);
                                
    wxDOMNode SetNamedItem(wxDOMNode& arg);
    wxDOMNode SetNamedItemNS(wxDOMNode& arg);
    
private:

    wxDOMNamedNodeMapData* m_data;
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMAttr class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMAttr
// Category: DOM
// Derives: wxDOMNode
// Description: Encapsulates a DOM attribute.
// Remarks: The wxDOMAttr class encapsulates a DOM attribute.

class wxDOMAttr : public wxDOMNode
{
public:

    wxDOMAttr();
    wxDOMAttr(const wxDOMNode& node);
    wxDOMAttr& operator=(const wxDOMNode& c);

    virtual bool IsOk() const;

    // DOMAttr interface
    
    wxString GetName();
    bool GetSpecified();
    wxString GetValue();
    wxDOMElement GetOwnerElement();
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMElement
// Category: DOM
// Derives: wxDOMNode
// Description: Encapsulates a DOM element.
// Remarks: The wxDOMElement class encapsulates a DOM element.

class wxDOMElement : public wxDOMNode
{
public:

    wxDOMElement();
    wxDOMElement(const wxDOMNode& node);
    wxDOMElement& operator=(const wxDOMNode& c);

    virtual bool IsOk() const;
    
    // DOMElement interface
    
    wxString GetTagName();
    wxString GetAttribute(const wxString& name);

    void SetAttribute(const wxString& name, const wxString& value);
    void RemoveAttribute(const wxString& name);

    wxDOMAttr GetAttributeNode(const wxString& name);
    wxDOMAttr SetAttributeNode(wxDOMAttr& new_attr);
    wxDOMAttr RemoveAttributeNode(wxDOMAttr& old_attr);

    wxDOMNodeList GetElementsByTagName(const wxString& name);

    wxString GetAttributeNS(const wxString& namespace_uri, const wxString& local_name);
    void SetAttributeNS(const wxString& namespace_uri, const wxString& qualified_name, const wxString& value);

    void RemoveAttributeNS(const wxString& namespace_uri, const wxString& local_name);
    wxDOMAttr GetAttributeNodeNS(const wxString& namespace_uri, const wxString& local_name);
    wxDOMAttr SetAttributeNodeNS(wxDOMAttr& new_attr);

    wxDOMNodeList GetElementsByTagNameNS(const wxString& namespace_uri, const wxString& local_name);

    bool HasAttribute(const wxString& name);
    bool HasAttributeNS(const wxString& namespace_uri, const wxString& local_name);
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMText class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMText
// Category: DOM
// Derives: wxDOMNode
// Description: Encapsulates a DOM text node.
// Remarks: The wxDOMText class encapsulates a DOM text node.

class wxDOMText : public wxDOMNode
{
public:

    wxDOMText();
    wxDOMText(const wxDOMNode& node);
    wxDOMText& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;
    
    void SetData(const wxString& data);
    wxString GetData();
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMDocument class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMDocument
// Category: DOM
// Derives: wxDOMNode
// Description: Encapsulates a DOM document node.
// Remarks: The wxDOMDocument class encapsulates a DOM document node.

class wxDOMDocument : public wxDOMNode
{
public:

    wxDOMDocument();
    wxDOMDocument(const wxDOMNode& node);
    wxDOMDocument& operator=(const wxDOMNode& c);

    virtual bool IsOk() const;

    // DOMDocument interface

    //NS_IMETHOD GetDoctype(nsIDOMDocumentType * *aDoctype);
    //NS_IMETHOD GetImplementation(nsIDOMDOMImplementation * *aImplementation);
    wxDOMElement GetDocumentElement();
    
    wxDOMElement CreateElement(const wxString& tag_name);
    //NS_IMETHOD CreateDocumentFragment(nsIDOMDocumentFragment **_retval);
    wxDOMText CreateTextNode(const wxString& data);
    //NS_IMETHOD CreateComment(const nsAString & data, nsIDOMComment **_retval);
    
    //NS_IMETHOD CreateCDATASection(const nsAString & data, nsIDOMCDATASection **_retval);
    //NS_IMETHOD CreateProcessingInstruction(const nsAString & target, const nsAString & data, nsIDOMProcessingInstruction **_retval);
    wxDOMAttr CreateAttribute(const wxString& name);
    //NS_IMETHOD CreateEntityReference(const nsAString & name, nsIDOMEntityReference **_retval);
    
    wxDOMNodeList GetElementsByTagName(const wxString& name);

    wxDOMNode ImportNode(wxDOMNode& arg, bool deep);
    
    wxDOMElement CreateElementNS(const wxString& namespace_uri, const wxString& local_name);
    wxDOMAttr CreateAttributeNS(const wxString& namespace_uri, const wxString& local_name);
    
    wxDOMNodeList GetElementsByTagNameNS(const wxString& namespace_uri, const wxString& local_name);
    wxDOMElement GetElementById(const wxString& id);
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLElement
// Category: DOM
// Derives: wxDOMElement
// Description: Encapsulates a DOM HTML element.
// Remarks: The wxDOMHTMLElement class encapsulates a DOM HTML element.

class wxDOMHTMLElement : public wxDOMElement
{
public:

    wxDOMHTMLElement();
    wxDOMHTMLElement(const wxDOMNode& node);
    wxDOMHTMLElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // DOMHTMLElement interface

    wxString GetId();
    void SetId(const wxString& value);

    wxString GetTitle();
    void SetTitle(const wxString& value);

    wxString GetLang();
    void SetLang(const wxString& value);

    wxString GetDir();
    void SetDir(const wxString& value);

    wxString GetClassName();
    void SetClassName(const wxString& value);

public:

    // these are shortcut accessors to those DOM elements
    // that have a 'value' property;  the IDOMHTMLElement
    // doesn't normally have these calls
    
    wxString GetValue();
    void SetValue(const wxString& value);
    bool HasValueProperty() const;
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLAnchorElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLAnchorElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM anchor element.
// Remarks: The wxDOMHTMLAnchorElement class encapsulates a DOM anchor element.

class wxDOMHTMLAnchorElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLAnchorElement();
    wxDOMHTMLAnchorElement(const wxDOMNode& node);
    wxDOMHTMLAnchorElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // wxDOMHTMLAnchorElement interface

    wxString GetAccessKey();
    void SetAccessKey(const wxString& value);

    wxString GetCharset();
    void SetCharset(const wxString& value);

    wxString GetCoords();
    void SetCoords(const wxString& value);

    wxString GetHref();
    void SetHref(const wxString& value);

    wxString GetHreflang();
    void SetHreflang(const wxString& value);

    wxString GetName();
    void SetName(const wxString& value);

    wxString GetRel();
    void SetRel(const wxString& value);

    wxString GetRev();
    void SetRev(const wxString& value);

    wxString GetShape();
    void SetShape(const wxString& value);

    int GetTabIndex();
    void SetTabIndex(int index);

    wxString GetTarget();
    void SetTarget(const wxString& value);

    wxString GetType();
    void SetType(const wxString& value);

    void Blur();
    void Focus();
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLButtonElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLButtonElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML button element.
// Remarks: The wxDOMHTMLButtonElement class encapsulates a DOM HTML 
//     button element.

class wxDOMHTMLButtonElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLButtonElement();
    wxDOMHTMLButtonElement(const wxDOMNode& node);
    wxDOMHTMLButtonElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // DOMHTMLButtonElement interface

    // NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm);

    wxString GetAccessKey();
    void SetAccessKey(const wxString& value);
    
    bool GetDisabled();
    void SetDisabled(bool value);
    
    wxString GetName();
    void SetName(const wxString& value);
    
    int GetTabIndex();
    void SetTabIndex(int index);
    
    wxString GetType();

    // following interface items are on base class wxDOMHTMLElement
    // NS_IMETHOD GetValue(nsAString& value);
    // NS_IMETHOD SetValue(const nsAString& value);
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLInputElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLInputElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML input element.
// Remarks: The wxDOMHTMLInputElement class encapsulates a DOM HTML 
//     input element.

class wxDOMHTMLInputElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLInputElement();
    wxDOMHTMLInputElement(const wxDOMNode& node);
    wxDOMHTMLInputElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // DOMHTMLInputElement interface

    wxString GetDefaultValue();
    void SetDefaultValue(const wxString& value);

    bool GetDefaultChecked();
    void SetDefaultChecked(bool value);

    // NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm); \

    wxString GetAccept();
    void SetAccept(const wxString& value);

    wxString GetAccessKey();
    void SetAccessKey(const wxString& value);

    wxString GetAlign();
    void SetAlign(const wxString& value);

    wxString GetAlt();
    void SetAlt(const wxString& value);

    bool GetChecked();
    void SetChecked(bool value);

    bool GetDisabled();
    void SetDisabled(bool value);

    int GetMaxLength();
    void SetMaxLength(int value);

    wxString GetName();
    void SetName(const wxString& value);

    bool GetReadOnly();
    void SetReadOnly(bool value);

    int GetSize();
    void SetSize(int value);

    wxString GetSrc();
    void SetSrc(const wxString& value);

    int GetTabIndex();
    void SetTabIndex(int value);

    wxString GetType();
    void SetType(const wxString& value);

    wxString GetUseMap();
    void SetUseMap(const wxString& value);

    // following interface items are on base class wxDOMHTMLElement
    // NS_IMETHOD GetValue(nsAString& value);
    // NS_IMETHOD SetValue(const nsAString& value);
    
    void Blur();
    void Focus();
    void Select();
    void Click();
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLLinkElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLLinkElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML link element.
// Remarks: The wxDOMHTMLLinkElement class encapsulates a DOM HTML link 
//     element.

class wxDOMHTMLLinkElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLLinkElement();
    wxDOMHTMLLinkElement(const wxDOMNode& node);
    wxDOMHTMLLinkElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // wxDOMHTMLLinkElement interface

    bool GetDisabled();
    void SetDisabled(bool value);
    
    wxString GetCharset();
    void SetCharset(const wxString& value);
    
    wxString GetHref();
    void SetHref(const wxString& value);
    
    wxString GetHreflang();
    void SetHreflang(const wxString& value);
    
    wxString GetMedia();
    void SetMedia(const wxString& value);
    
    wxString GetRel();
    void SetRel(const wxString& value);
    
    wxString GetRev();
    void SetRev(const wxString& value);
    
    wxString GetTarget();
    void SetTarget(const wxString& value);
    
    wxString GetType();
    void SetType(const wxString& value);
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLOptionElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLOptionElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML option element.
// Remarks: The wxDOMHTMLOptionElement class encapsulates a DOM HTML 
//     option element.

class wxDOMHTMLOptionElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLOptionElement();
    wxDOMHTMLOptionElement(const wxDOMNode& node);
    wxDOMHTMLOptionElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // DOMHTMLOptionElement interface

    // NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm);
    
    bool GetDefaultSelected();
    void SetDefaultSelected(bool value);

    wxString GetText();
    int GetIndex();

    bool GetDisabled();
    void SetDisabled(bool value);

    wxString GetLabel();
    void SetLabel(const wxString& value);

    bool GetSelected();
    void SetSelected(bool value);

    // following interface items are on base class wxDOMHTMLElement
    // NS_IMETHOD GetValue(nsAString& value);
    // NS_IMETHOD SetValue(const nsAString& value);
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLParamElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLParamElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML param element.
// Remarks: The wxDOMHTMLParamElement class encapsulates a DOM HTML 
//     param element.

class wxDOMHTMLParamElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLParamElement();
    wxDOMHTMLParamElement(const wxDOMNode& node);
    wxDOMHTMLParamElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // wxDOMHTMLParamElement interface

    wxString GetName();
    void SetName(const wxString& name);
  
    wxString GetType();
    void SetType(const wxString& type);

    // following interface items are on base class wxDOMHTMLElement
    // NS_IMETHOD GetValue(nsAString& value);
    // NS_IMETHOD SetValue(const nsAString& value);
  
    wxString GetValueType();
    void SetValueType(const wxString& valuetype);
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLSelectElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLSelectElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML select element.
// Remarks: The wxDOMHTMLSelectElement class encapsulates a DOM HTML 
//     select element.

class wxDOMHTMLSelectElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLSelectElement();
    wxDOMHTMLSelectElement(const wxDOMNode& node);
    wxDOMHTMLSelectElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // DOMHTMLSelectElement interface
    
    wxString GetType();

    int GetSelectedIndex();
    void SetSelectedIndex(int index);

    // following interface items are on base class wxDOMHTMLElement
    // NS_IMETHOD GetValue(nsAString& value);
    // NS_IMETHOD SetValue(const nsAString& value);

    int GetLength();
    void SetLength(int length);

    // NS_IMETHOD GetForm(nsIDOMHTMLFormElement** form);
    // NS_IMETHOD GetOptions(nsIDOMHTMLOptionsCollection** options);

    bool GetDisabled();
    void SetDisabled(bool value);

    bool GetMultiple();
    void SetMultiple(bool value);

    wxString GetName();
    void SetName(const wxString& value);

    int GetSize();
    void SetSize(int value);

    int GetTabIndex();
    void SetTabIndex(int index);
 
    void Add(const wxDOMHTMLElement& element,
             const wxDOMHTMLElement& before);
    void Remove(int index);
    void Blur();
    void Focus();
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMHTMLTextAreaElement class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMHTMLTextAreaElement
// Category: DOM
// Derives: wxDOMHTMLElement
// Description: Encapsulates a DOM HTML text area element.
// Remarks: The wxDOMHTMLTextAreaElement class encapsulates a DOM HTML text 
//     area element.

class wxDOMHTMLTextAreaElement : public wxDOMHTMLElement
{
public:

    wxDOMHTMLTextAreaElement();
    wxDOMHTMLTextAreaElement(const wxDOMNode& node);
    wxDOMHTMLTextAreaElement& operator=(const wxDOMNode& c);
    
    virtual bool IsOk() const;

    // DOMHTMLTextAreaElement interface

    wxString GetDefaultValue();
    void SetDefaultValue(const wxString& value);

    //GetForm(nsIDOMHTMLFormElement * *aForm);

    wxString GetAccessKey();
    void SetAccessKey(const wxString& value);

    int GetCols();
    void SetCols(int value);

    bool GetDisabled();
    void SetDisabled(bool value);

    wxString GetName();
    void SetName(const wxString& value);

    bool GetReadOnly();
    void SetReadOnly(bool value);

    int GetRows();
    void SetRows(int value);

    int GetTabIndex();
    void SetTabIndex(int value);

    wxString GetType();

    // following interface items are on base class wxDOMHTMLElement
    // NS_IMETHOD GetValue(nsAString& value);
    // NS_IMETHOD SetValue(const nsAString& value);

    void Blur();
    void Focus();
    void Select();
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMEvent class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMEvent
// Category: DOM
// Description: Encapsulates a DOM event.
// Remarks: The wxDOMEvent class encapsulates a DOM event.

class wxDOMEvent
{
public:

    wxDOMEvent();
    ~wxDOMEvent();
    wxDOMEvent(const wxDOMEvent& c);
    wxDOMEvent& operator=(const wxDOMEvent& c);
    void assign(const wxDOMEvent& c);

    virtual bool IsOk() const;

    wxString GetType();
    // wxDOMEventTarget GetTarget();

public:

    wxDOMEventData* m_data;
};




///////////////////////////////////////////////////////////////////////////////
//  wxDOMMouseEvent class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxDOMMouseEvent
// Category: DOM
// Derives: wxDOMEvent
// Description: Encapsulates a DOM mouse event.
// Remarks: The wxDOMMouseEvent class encapsulates a DOM mouse event.

class wxDOMMouseEvent : public wxDOMEvent
{
public:

    wxDOMMouseEvent();
    wxDOMMouseEvent(const wxDOMEvent& node);
    wxDOMMouseEvent& operator=(const wxDOMEvent& c);
    
    virtual bool IsOk() const;
    
    long GetScreenX();
    long GetScreenY();
    long GetClientX();
    long GetClientY();
    bool GetCtrlKey();
    bool GetShiftKey();
    bool GetAltKey();
    bool GetMetaKey();
    unsigned short GetButton();
    // wxDOMEventTarget GetRelatedTarget();
};


#endif

